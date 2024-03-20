# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: D:\project\vitis_final\single_udp_test_system\_ide\scripts\systemdebugger_cameralink_udp_system_standalone.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source D:\project\vitis_final\single_udp_test_system\_ide\scripts\systemdebugger_cameralink_udp_system_standalone.tcl
# 
connect -url tcp:127.0.0.1:3121
source C:/Xilinx_vitis/Vitis/2021.2/scripts/vitis/util/zynqmp_utils.tcl
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Digilent JTAG-SMT2 210251251359" && level==0 && jtag_device_ctx=="jsn-JTAG-SMT2-210251251359-04721093-0"}
fpga -file D:/project/vitis_final/single_udp_test/_ide/bitstream/cameralink_display_wrapper_1_24_2.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw D:/project/vitis_final/lwip_modi_3/export/lwip_modi_3/hw/cameralink_display_wrapper_1_24_2.xsa -mem-ranges [list {0x80000000 0xbfffffff} {0x400000000 0x5ffffffff} {0x1000000000 0x7fffffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
set mode [expr [mrd -value 0xFF5E0200] & 0xf]
targets -set -nocase -filter {name =~ "*A53*#0"}
rst -processor
dow D:/project/vitis_final/lwip_modi_3/export/lwip_modi_3/sw/lwip_modi_3/boot/fsbl.elf
set bp_3_18_fsbl_bp [bpadd -addr &XFsbl_Exit]
con -block -timeout 60
bpremove $bp_3_18_fsbl_bp
targets -set -nocase -filter {name =~ "*A53*#0"}
rst -processor
dow D:/project/vitis_final/single_udp_test/Debug/single_udp_test.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "*A53*#0"}
con
