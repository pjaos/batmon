# REST interface documentation.
The unit has a REST interface. The following documentation details
this interface. The examples here show the unit IP address which
may change.

#Calling rest IF

curl http://192.168.0.34/rpc/debug -d '{"level":2}'
2

curl http://192.168.0.34/rpc/factorydefault

curl http://192.168.0.34/rpc/load -d '{"on":1}'
0
curl http://192.168.0.34/rpc/load -d '{"on":0}'
1

curl http://192.168.0.41/rpc/set_amps_cal -d '{"codes_per_amp":5112}'
1512
5135

curl http://192.168.0.41/rpc/set_pvolts_cal -d '{"codes_per_volt":691}'
691

curl http://192.168.0.41/rpc/set_nvolts_cal -d '{"codes_per_volt":691}'
691

curl http://192.168.0.34/rpc/set_max_battery_charge -d '{"level":2}'
2

curl http://192.168.0.34/rpc/get_state
{"amps":0.001,"volts":22.006,"temperature":154.5}

### This causes the syslog output to be sent to the server the YView AYT message was received from
curl http://192.168.0.34/rpc/ydev.update_syslog
null


# Reboot the device
curl http://192.168.0.34/rpc/sys.reboot

curl http://192.168.0.34/rpc/rpc.list
["debug","update_syslog","factorydefault","WiFi.SetupComplete","WiFi.Scan","I2C.WriteRegW","I2C.WriteRegB","I2C.ReadRegW","I2C.ReadRegB","I2C.Write","I2C.Read","I2C.Scan","FS.Umount","FS.Mount","FS.Mkfs","FS.Rename","FS.Remove","FS.Put","FS.Get","FS.ListExt","FS.List","Config.Save","Config.Set","Config.Get","Sys.SetDebug","Sys.GetInfo","Sys.Reboot","RPC.Ping","RPC.Describe","RPC.List"]

curl http://192.168.0.34/rpc/Sys.GetInfo
{"id": "esp32_BA8DB5", "app": "esp32.git", "fw_version": "1.0", "fw_id": "2025629-085437/g2805f19-master-dirty", "mg_version": "2.19.1", "mg_id": "2025629-072504/2.19.1-gd52032f", "mac": "B4E62DBA8DB5", "arch": "esp32", "uptime": 540, "public_key": null, "ram_size": 281872, "ram_free": 221412, "ram_min_free": 208668, "fs_size": 233681, "fs_free": 144576,"wifi": {"sta_ip": "192.168.0.34", "ap_ip": "", "status": "got ip", "ssid": "airstorm11"}}

curl http://192.168.0.34/rpc/Config.Get
{"debug":{"udp_log_addr":"","udp_log_level":3,"mbedtls_level":1,"level":0,"file_level":"","event_level":2,"stdout_uart":0,"stderr_uart":0,"factory_reset_gpio":-1,"mg_mgr_hexdump_file":""},"i2c":{"unit_no":0,"enable":true,"freq":400000,"debug":false,"sda_gpio":22,"scl_gpio":21},"i2c1":{"unit_no":1,"enable":false,"freq":400000,"debug":false,"sda_gpio":22,"scl_gpio":23},"device":{"id":"esp32_BA8DB5","license":"","mac":"","public_key":"","sn":""},"sys":{"tz_spec":"","wdt_timeout":30,"pref_ota_lib":""},"conf_acl":"*","http":{"enable":true,"listen_addr":"80","document_root":"/","index_files":"","ssl_cert":"","ssl_key":"","ssl_ca_cert":"","upload_acl":"*","hidden_files":"","auth_domain":"","auth_file":"","extra_headers":""},"update":{"timeout":600,"commit_timeout":0,"url":"","interval":0,"extra_http_headers":"","ssl_ca_file":"ca.pem","ssl_client_cert_file":"","ssl_server_name":"","enable_post":true},"rpc":{"enable":true,"http_enable":true,"service_sys_enable":true,"max_frame_size":4096,"max_queue_length":25,"default_out_channel_idle_close_timeout":10,"acl_file":"","auth_domain":"RPC","auth_file":"","uart":{"uart_no":0,"baud_rate":115200,"fc_type":2,"dst":""}},"spi":{"enable":false,"debug":false,"unit_no":3,"miso_gpio":19,"mosi_gpio":23,"sclk_gpio":18,"cs0_gpio":5,"cs1_gpio":-1,"cs2_gpio":-1},"syslog":{"url":"udp://192.168.0.8:514","hostname":"BATMON1"},"wifi":{"ap":{"enable":false,"ssid":"Mongoose_??????","pass":"12345678","hidden":false,"channel":6,"max_connections":10,"ip":"192.168.4.1","netmask":"255.255.255.0","gw":"192.168.4.1","dhcp_start":"192.168.4.2","dhcp_end":"192.168.4.100","trigger_on_gpio":-1,"disable_after":0,"hostname":"","keep_enabled":true,"bandwidth_20mhz":false,"protocol":"BGN"},"sta":{"enable":true,"ssid":"airstorm11","pass":"XXXXXXXX","user":"","anon_identity":"","cert":"","key":"","ca_cert":"","ip":"","netmask":"","gw":"","nameserver":"","dhcp_hostname":"","protocol":"BGN","listen_interval_ms":0},"sta1":{"enable":false,"ssid":"","pass":"","user":"","anon_identity":"","cert":"","key":"","ca_cert":"","ip":"","netmask":"","gw":"","nameserver":"","dhcp_hostname":"","protocol":"BGN","listen_interval_ms":0},"sta2":{"enable":false,"ssid":"","pass":"","user":"","anon_identity":"","cert":"","key":"","ca_cert":"","ip":"","netmask":"","gw":"","nameserver":"","dhcp_hostname":"","protocol":"BGN","listen_interval_ms":0},"sta_cfg_idx":0,"sta_connect_timeout":30,"sta_ps_mode":0,"sta_all_chan_scan":true},"platform":{"wifi_button_gpio":0,"wifi_led_gpio":2},"user":{"setup_mode":0,"wifi_mode":1,"product_id":"BATMON"},"ydev":{"product_id":"BATMON","unit_name":"BATMON1","group_name":"","enable_syslog":false},"ydevayth":{"hashtag":"-!#8[dkG^v's!dRznE}6}8sP9}QoIR#?O&pg)Qra"},"board":{"led1":{"pin":13,"active_high":true},"led2":{"pin":-1,"active_high":true},"led3":{"pin":-1,"active_high":true},"btn1":{"pin":0,"pull_up":true},"btn2":{"pin":-1,"pull_up":false},"btn3":{"pin":-1,"pull_up":false}}}
