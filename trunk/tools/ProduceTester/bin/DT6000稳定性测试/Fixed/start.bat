copy  \FlashDisk\ReliabilityTestCE\dll\epcSysInfo.dll    \windows\epcSysInfo.dll
copy  \FlashDisk\ReliabilityTestCE\dll\ControlCAN.dll    \windows\ControlCAN.dll
copy  \FlashDisk\ReliabilityTestCE\dll\epcBuzzerLib.dll  \windows\epcBuzzerLib.dll
copy  \FlashDisk\ReliabilityTestCE\dll\epcLedLib.dll     \windows\epcLedLib.dll
copy  \FlashDisk\ReliabilityTestCE\dll\epcIoLib.dll      \windows\epcIoLib.dll

copy  \FlashDisk\ReliabilityTestCE\ReliabilityTestNoEth.exe     \ReliabilityTestNoEth.exe
copy  \FlashDisk\ReliabilityTestCE\config.ini     		\config.ini

md    \config
copy  \FlashDisk\ReliabilityTestCE\config\*.*      		\config\
md    \pcap
copy  \FlashDisk\ReliabilityTestCE\pcap\sv92_test.pcap      		\pcap\sv92_test.pcap
copy  \FlashDisk\ReliabilityTestCE\pcap\GuoWang.txt      		\pcap\GuoWang.txt  
start \ReliabilityTestNoEth.exe