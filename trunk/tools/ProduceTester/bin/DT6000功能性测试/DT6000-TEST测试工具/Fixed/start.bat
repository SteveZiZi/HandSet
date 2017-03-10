copy  \FlashDisk\DT6000_TestCE\DT6000_FiberTestTool.exe    \DT6000_FiberTestTool.exe

md    \pcap
copy  \FlashDisk\DT6000_TestCE\pcap\sv92_test.pcap      		\pcap\sv92_test.pcap
copy  \FlashDisk\DT6000_TestCE\pcap\GuoWang.txt      		\pcap\GuoWang.txt 
copy  \FlashDisk\DT6000_TestCE\pcap\GOOSE.pcap      		\pcap\GOOSE.pcap 
start \DT6000_FiberTestTool.exe