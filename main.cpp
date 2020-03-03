#include "mbed.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "mbed_trace.h"
#include "https_request.h"

DigitalOut led(LED1);
InterruptIn button(USER_BUTTON);
Thread t;
EventQueue queue(5 * EVENTS_EVENT_SIZE);
Serial pc(USBTX, USBRX);
WiFiInterface *wifi;

const char SSL_CA_PEM[] = "-----BEGIN CERTIFICATE-----\n"
"MIIFXjCCBEagAwIBAgIQAqN7H8rlZzz0KddBKgxiVjANBgkqhkiG9w0BAQsFADBw\n"
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
"d3cuZGlnaWNlcnQuY29tMS8wLQYDVQQDEyZEaWdpQ2VydCBTSEEyIEhpZ2ggQXNz\n"
"dXJhbmNlIFNlcnZlciBDQTAeFw0xNzA0MTkwMDAwMDBaFw0yMDA2MjIxMjAwMDBa\n"
"MGsxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpDYWxpZm9ybmlhMRYwFAYDVQQHEw1T\n"
"YW4gRnJhbmNpc2NvMRUwEwYDVQQKEwxIZXJva3UsIEluYy4xGDAWBgNVBAMMDyou\n"
"aGVyb2t1YXBwLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMnN\n"
"1rH4ObYuY5NHb+xV4VV30Z+dxpkScWFWyi4yN86eTOA99A9FduC00UDgtMwaDD/r\n"
"6LVftlaY0ymKKa4+O24PEH5MrqR919yJNS2i+llJ0xQHthlKsW9GHuyyeOi6ZpK7\n"
"cPGuatR+Z5gwjGxPoMEyt1szMX8BUZpsH+/9SJ2zlC0pTm3P06QOZbkQEziQwtOW\n"
"Pj7285ZZUGHCcfb/1dYjJpxUism8p9TAIZnW6UgZ487r612HFJYFfS3bCHnS4Jsk\n"
"TsSUg7AuJz7Px46AAKuGJ8CoV9A1GHAq0ngFwph7i6tQIgAxSsniXHz94/JgZimj\n"
"5rn10dbXU28TjpXQdYECAwEAAaOCAfcwggHzMB8GA1UdIwQYMBaAFFFo/5CvAgd1\n"
"PMzZZWRiohK4WXI7MB0GA1UdDgQWBBSL9RPT/PsKraCvn2/CQsTYR0zL5TApBgNV\n"
"HREEIjAggg8qLmhlcm9rdWFwcC5jb22CDWhlcm9rdWFwcC5jb20wDgYDVR0PAQH/\n"
"BAQDAgWgMB0GA1UdJQQWMBQGCCsGAQUFBwMBBggrBgEFBQcDAjB1BgNVHR8EbjBs\n"
"MDSgMqAwhi5odHRwOi8vY3JsMy5kaWdpY2VydC5jb20vc2hhMi1oYS1zZXJ2ZXIt\n"
"ZzUuY3JsMDSgMqAwhi5odHRwOi8vY3JsNC5kaWdpY2VydC5jb20vc2hhMi1oYS1z\n"
"ZXJ2ZXItZzUuY3JsMEwGA1UdIARFMEMwNwYJYIZIAYb9bAEBMCowKAYIKwYBBQUH\n"
"AgEWHGh0dHBzOi8vd3d3LmRpZ2ljZXJ0LmNvbS9DUFMwCAYGZ4EMAQICMIGDBggr\n"
"BgEFBQcBAQR3MHUwJAYIKwYBBQUHMAGGGGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNv\n"
"bTBNBggrBgEFBQcwAoZBaHR0cDovL2NhY2VydHMuZGlnaWNlcnQuY29tL0RpZ2lD\n"
"ZXJ0U0hBMkhpZ2hBc3N1cmFuY2VTZXJ2ZXJDQS5jcnQwDAYDVR0TAQH/BAIwADAN\n"
"BgkqhkiG9w0BAQsFAAOCAQEAOtOltMw9Z7W3JdIqUIi6Q9JX/3q25T8FwkGgzVeG\n"
"/OdOzrha2QUxpp1wrG9YQV72jfrsUv3icogtfndhyJZUU+mpaQRNjsBniGEgJCWS\n"
"tVG2NQFHJtfk1f1eoayluR7iHZljSQKBgTj9qRZXJBmfz513NdwfiyiWmeqHHcor\n"
"WEkwjDQWq2m8fS1p1N7Le8ftXFPeeFZTCTX4brwGSWGyfZLaNlFuVj+iM4Dx35/N\n"
"1eoG59O14J40LAsxXGYtpz2heuO9bvcOLD0m/cl1ArgWZ7raAEo34qoiBLa5polJ\n"
"p94kEd/mq0LOwsEafVMchxnS408fZb9hn37PhZ5xLgFzjw==\n"
"-----END CERTIFICATE-----\n";

void dump_response(HttpResponse* res) {
    printf("Status: %d - %s\n", res->get_status_code(), res->get_status_message().c_str());

    printf("Headers:\n");
    for (size_t ix = 0; ix < res->get_headers_length(); ix++) {
        printf("\t%s: %s\n", res->get_headers_fields()[ix]->c_str(), res->get_headers_values()[ix]->c_str());
    }
    printf("\nBody (%lu bytes):\n\n%s\n", res->get_body_length(), res->get_body_as_string().c_str());
}

const char *sec2str(nsapi_security_t sec)
{
    switch (sec) {
        case NSAPI_SECURITY_NONE:
            return "None";
        case NSAPI_SECURITY_WEP:
            return "WEP";
        case NSAPI_SECURITY_WPA:
            return "WPA";
        case NSAPI_SECURITY_WPA2:
            return "WPA2";
        case NSAPI_SECURITY_WPA_WPA2:
            return "WPA/WPA2";
        case NSAPI_SECURITY_UNKNOWN:
        default:
            return "Unknown";
    }
}

int scan_wifi() {
    
	WiFiAccessPoint *ap;
    printf("Scan:\n");
    int count = wifi->scan(NULL,0);
    if (count <= 0) {
        printf("scan() failed with return value: %d\n", count);
        return 0;
    }

    /* Limit number of network arbitrary to 15 */
    count = count < 15 ? count : 15;
    ap = new WiFiAccessPoint[count];
    count = wifi->scan(ap, count);
    if (count <= 0) {
        printf("scan() failed with return value: %d\n", count);
        return 0;
    }

    for (int i = 0; i < count; i++) {
        printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\n", ap[i].get_ssid(),
               sec2str(ap[i].get_security()), ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
               ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5], ap[i].get_rssi(), ap[i].get_channel());
    }
    printf("%d networks available.\n", count);

    delete[] ap;   

    return count; 
}

void connect_to_wifi(){
	pc.printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
	int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
        pc.printf("\nConnection error: %d\n", ret);
        return;
    }
    pc.printf("Success\n\n");
    pc.printf("MAC: %s\n", wifi->get_mac_address());
    SocketAddress a;
    wifi->get_ip_address(&a);
    pc.printf("IP: %s\n", a.get_ip_address());
    wifi->get_netmask(&a);
    pc.printf("Netmask: %s\n", a.get_ip_address());
    wifi->get_gateway(&a);
    pc.printf("Gateway: %s\n", a.get_ip_address());
    pc.printf("RSSI: %d\n\n", wifi->get_rssi());
	pc.printf("\nDone\n");
}

 bool is_wifi_connected(){
	int ret = wifi->get_rssi();
    if(ret == 0)
		return false;
	else
		return true;
}

void pressed_handler() {
    int count;
    count = scan_wifi();
    if (count == 0) {
        pc.printf("No WIFI APs found - can't continue further.\n");
        return;
    }
    pc.printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
    int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
        pc.printf("\nConnection error: %d\n", ret);
        return;
    }

    pc.printf("Success\n\n");
    pc.printf("MAC: %s\n", wifi->get_mac_address());
    SocketAddress a;
    wifi->get_ip_address(&a);
    pc.printf("IP: %s\n", a.get_ip_address());
    wifi->get_netmask(&a);
    pc.printf("Netmask: %s\n", a.get_ip_address());
    wifi->get_gateway(&a);
    pc.printf("Gateway: %s\n", a.get_ip_address());
    pc.printf("RSSI: %d\n\n", wifi->get_rssi());
    pc.printf("\nDone\n");
	
	float temp_value;
	float humid_value;
	while(1) {
		if(!is_wifi_connected()){
			printf("No connection to WIFI\n");
			connect_to_wifi();
		}else{
			
			temp_value = BSP_TSENSOR_ReadTemp();
			printf("\nTEMPERATURE = %.2f degC\n", temp_value);
			humid_value = BSP_HSENSOR_ReadHumidity();
			printf("HUMIDITY    = %.2f %%\n", humid_value);
			;
			
			{
				char link[128];
				sprintf(link,"https://iot-humidity-and-temperature.herokuapp.com/input?temp=%.2f&humid=%.2f", temp_value, humid_value);
				printf(link);
				HttpsRequest* get_req = new HttpsRequest(wifi, SSL_CA_PEM, HTTP_GET, link);
				HttpResponse* get_res = get_req->send();
				if (!get_res) {
					printf("\nHttpRequest failed (error code %d)\n", get_req->get_error());
				}
				delete get_req;
			}
			
			{
				HttpsRequest* post_req = new HttpsRequest(wifi, SSL_CA_PEM, HTTP_POST, "https://iot-humidity-and-temperature.herokuapp.com/input");
				post_req->set_header("Content-Type", "application/json");
				char body[100];
				sprintf(body,"{\"humid\":\%.2f,\"temp\":\%.2f}", humid_value, temp_value);
				HttpResponse* get_res1 = post_req->send(body, strlen(body));
				if (!get_res1) {
					 printf("\nHttpRequest failed (error code %d)\n", post_req->get_error());
				}
				 delete post_req;
			}
		
		}    
	}ThisThread::sleep_for(30000);
}

int main(){
	wifi = WiFiInterface::get_default_instance();
    if (!wifi) {
        printf("ERROR: No WiFiInterface found.\n");
        return -1;
    }
	
	led = 1;
    BSP_TSENSOR_Init();
    BSP_HSENSOR_Init();
	t.start(callback(&queue, &EventQueue::dispatch_forever));
    button.fall(queue.event(pressed_handler));
	printf("Start\n");
     while(1) {
        led = !led;
        ThisThread::sleep_for(500);
		}
   }