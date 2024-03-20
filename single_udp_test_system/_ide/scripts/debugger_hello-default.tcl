# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: E:\new\ps_code_v1.0\vitis_mpsoc_new\single_udp_test_system\_ide\scripts\debugger_hello-default.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source E:\new\ps_code_v1.0\vitis_mpsoc_new\single_udp_test_system\_ide\scripts\debugger_hello-default.tcl
# 
connect -url tcp:127.0.0.1:3121
source D:/installPath/vitis/Vitis/2021.2/scripts/vitis/util/zynqmp_utils.tcl
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Platform Cable USB 00000000000000" && level==0 && jtag_device_ctx=="jsn-DLC9LP-00000000000000-04721093-0"}
fpga -file E:/new/ps_code_v1.0/vitis_mpsoc_new/single_udp_test/_ide/bitstream/cameralink_display_wrapper.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw E:/new/ps_code_v1.0/vitis_mpsoc_new/cameralink_display_wrapper_mpsoc/export/cameralink_display_wrapper_mpsoc/hw/cameralink_display_wrapper.xsa -mem-ranges [list {0x80000000 0xbfffffff} {0x400000000 0x5ffffffff} {0x1000000000 0x7fffffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
set mode [expr [mrd -value 0xFF5E0200] & 0xf]
targets -set -nocase -filter {name =~ "*A53*#0"}
rst -processor
dow E:/new/ps_code_v1.0/vitis_mpsoc_new/cameralink_display_wrapper_mpsoc/export/cameralink_display_wrapper_mpsoc/sw/cameralink_display_wrapper_mpsoc/boot/fsbl.elf
set bp_43_28_fsbl_bp [bpadd -addr &XFsbl_Exit]
con -block -timeout 60
bpremove $bp_43_28_fsbl_bp
targets -set -nocase -filter {name =~ "*A53*#0"}
rst -processor
dow E:/new/ps_code_v1.0/vitis_mpsoc_new/single_udp_test/Debug/single_udp_test.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "*A53*#0"}
con
