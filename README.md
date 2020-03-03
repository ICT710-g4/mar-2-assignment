
# ICT710: Assignment3, Mar 2.

These programs are a part of Software Design for Embedded System laboratory. This project use the `DISCO L475VG IOT01a` board.

## Server

- `https://iot-humidity-and-temperature.herokuapp.com/input`, POST method
	- param: float temp, float humid
	- put data into database
	
- `https://iot-humidity-and-temperature.herokuapp.com/summary`, get method
	- show all data in database

## Members

| Name | StudentID |
|--|--|
| Isada Sukprapa| 6222040302 |
| Narusorn Sirivon  | 6222040310 |
| Menghorng Bun | 6222040096 |

All of us are students in TAIST-ICTES program, sem. 2/2019

## Objective

1.  modify the flask-ws-register to work with Postgres database

2. study and make mbed code to read temperature and humidity, then send data to server

3. make server code for /update and /query and deploy on Heroku cloud

## Libraries

- mbed-os (**version 5.14**)
- DISCO_L475VG_IOT01-Sensors-BSP
	- `mbed add https://os.mbed.com/teams/ST/code/DISCO_L475VG_IOT01-Sensors-BSP/`

- mbed-http
	- `mbed add https://os.mbed.com/teams/sandbox/code/http-example/`

- wifi-ism43362
	- `mbed add https://github.com/ARMmbed/wifi-ism43362`


---






