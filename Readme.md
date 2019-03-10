# Happy3D

### This is software to manage SLS 3D printer, build in TalTech

This is a Qt port of original Happy3D SW

Author on this SW - Aleksandr Bobyrev

SW date: 03.03.2019

Screenshot of current version (0d8cd71) 

Update screenshot, version 1.0.5

![](https://github.com/Anaga/Happy3D-Qt/blob/master/Img/ScrinshotV5.png)

![](C:\Users\aleks\Documents\GitHub\Happy3D-Qt\Img\ScrinshotV1.0.2.png)

Testing the GitKraken under linux. Roman.

Testing the GitKraken under linux-2. From 3D printer PC. Roman.



##### ET - Exposure Time at one point in [microseconds]

sent: "#del=100"  
response: "DELAY_OK	100 (us)"    

##### PD - Point Distance between two points in [micrometres]

sent: "#step=100"  
response: STEP_OK

##### LP - Laser power in [percent]

sent: "#pow=20"  
responce: POWER_OK

##### CIRCLE - Draw circle (radius),(x),(y),(start degree),(end degree) in [centimetres]

sent: "#circle=1,2,2,0,180"  
response: CIRCLE_OK

##### LINE - Draw line (x0),(y0),(x1),(y1) in [centimetres]

sent: "#line=0,0,10,10"
response: LINE_OK

log example
`[2019-03-10 15:09:26.795] [Comm Laser] [info] SendCommand`
`[2019-03-10 15:09:26.795] [Comm Laser] [info] m_writeData : #del=100`
`[2019-03-10 15:09:26.806] [Comm Laser] [info] handleTimeout DELAY_OK	100 (us)`

`[2019-03-10 15:09:27.795] [Comm Laser] [info] SendCommand`
`[2019-03-10 15:09:27.795] [Comm Laser] [info] m_writeData : #step=100`
`[2019-03-10 15:09:27.816] [Comm Laser] [info] handleTimeout STEP_OK`

`[2019-03-10 15:09:28.795] [Comm Laser] [info] SendCommand`
`[2019-03-10 15:09:28.795] [Comm Laser] [info] m_writeData : #pow=20`
`[2019-03-10 15:09:28.806] [Comm Laser] [info] handleTimeout POWER_OK`

`[2019-03-10 15:09:29.795] [Comm Laser] [info] SendCommand`
`[2019-03-10 15:09:29.795] [Comm Laser] [info] m_writeData : #circle=1,2,2,0,180`
`[2019-03-10 15:09:29.877] [Comm Laser] [info] handleTimeout CIRCLE_OK`

`[2019-03-10 15:49:53.041] [Comm Laser] [info] SendCommand`
`[2019-03-10 15:49:53.041] [Comm Laser] [info] m_writeData : #line=0,10,10,0`
`[2019-03-10 15:49:56.947] [Comm Laser] [info] handleTimeout LINE_OK`
