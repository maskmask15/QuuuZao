# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct E:\new\ps_code_v1.0\vitis_mpsoc_new\lwip_modi_3\platform.tcl
# 
# OR launch xsct and run below command.
# source E:\new\ps_code_v1.0\vitis_mpsoc_new\lwip_modi_3\platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {lwip_modi_3}\
-hw {E:\new\pl_v1.0\cameralink_display_base\cameralink_display_wrapper_1_24_2.xsa}\
-proc {psu_cortexa53_0} -os {standalone} -arch {64-bit} -fsbl-target {psu_cortexa53_0} -out {E:/new/ps_code_v1.0/vitis_mpsoc_new}

platform write
platform generate -domains 
platform active {lwip_modi_3}
bsp reload
bsp setlib -name xilffs -ver 4.6
bsp setlib -name lwip211 -ver 2.0
bsp config mem_size "1485760"
bsp write
bsp reload
catch {bsp regenerate}
bsp reload
bsp config use_lfn "1"
bsp config use_strfunc "2"
bsp write
bsp reload
catch {bsp regenerate}
bsp config memp_n_pbuf "2048"
bsp config memp_n_sys_timeout "8"
bsp config memp_n_udp_pcb "256"
bsp config pbuf_debug "true"
bsp config udp_debug "true"
bsp config pbuf_pool_bufsize "3000"
bsp config pbuf_pool_size "16384"
bsp write
bsp reload
catch {bsp regenerate}
domain create -name {standalone_on_psu_cortexa53_1} -os {standalone} -proc {psu_cortexa53_1} -arch {64-bit} -display-name {standalone_on_psu_cortexa53_1} -desc {} -runtime {cpp}
platform generate -domains 
platform write
domain -report -json
bsp reload
bsp setlib -name xilffs -ver 4.6
bsp config use_lfn "1"
bsp config use_strfunc "2"
bsp write
bsp reload
catch {bsp regenerate}
platform generate
domain active {standalone_domain}
bsp reload
catch {platform remove cameralink_display_wrapper_mpsoc}
catch {platform remove lwip_modi}
platform active {lwip_modi_3}
bsp reload
domain active {standalone_on_psu_cortexa53_1}
bsp reload
bsp config use_strfunc "3"
bsp write
bsp reload
catch {bsp regenerate}
bsp config use_strfunc "2"
bsp write
bsp reload
catch {bsp regenerate}
domain active {standalone_domain}
bsp reload
domain active {zynqmp_fsbl}
bsp reload
domain active {standalone_domain}
bsp reload
bsp reload
bsp reload
platform generate -domains standalone_on_psu_cortexa53_1 
catch {platform remove lwip_revers}
platform active {lwip_modi_3}
platform generate -domains 
platform generate -domains standalone_domain 
platform generate -domains standalone_domain 
platform generate -domains standalone_domain 
platform active {lwip_modi_3}
platform active {lwip_modi_3}
domain active {zynqmp_fsbl}
domain active {zynqmp_fsbl}
