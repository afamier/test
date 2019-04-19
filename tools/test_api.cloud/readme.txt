----------------------------------------------------------------
Mibox Test Server:    58.96.170.163
Xml-Service API Port: 3030/udp
----------------------------------------------------------------
1. Heartbeat
.xml
   Param: NA

2. GetTrapInfo.xml
   Param: NA

3. SetTrapInfo.xml
   Param: { AdminStatus, IpAddr, PortNo }
---------------------------------------------------------------
10.GetMiboxList.xml
   Param: { BeginDeviceId, Order, MaxGetCount }

11.GetMiboxInfo.xml
   Param: { DeviceSn }
   Info:  { DeviceSn, Alias, AdminStatus, RunStatus, ...
            ServiceType, ServiceStatus, BalDays, 
            TotalUsedKB, TotalRecvKB, TotalSendKB, TotalUsedMin,
            DayUsedKB, DayRecvKB, DaySendKB, DayUsedMin,
            CurUsedKB, CurRecvKB, CurSendKB, CurUsedMin
          }

12.SetMiboxInfo.xml
   Param: { DeviceSn, Alias, AdminStatus, ServiceType }

13.SetMiboxAction.xml
   Param: { DeviceSn, RESET_BAL, BalDays, ExpiredTime }

14.GetMiboxDdr.xml
   Param: { DeviceSn, BeginDataSn, Order, MaxGetCount }

