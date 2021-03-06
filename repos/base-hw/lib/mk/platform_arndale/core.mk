#
# \brief  Build config for Genodes core process
# \author Stefan Kalkowski
# \date   2015-02-09
#

# add include paths
INC_DIR += $(REP_DIR)/src/core/include/spec/arm_v7/virtualization

# add C++ sources
SRC_CC += spec/arndale/board.cc
SRC_CC += spec/arndale/pic.cc
SRC_CC += spec/arndale/platform_services.cc
SRC_CC += spec/arm_v7/kernel/vm_thread.cc
SRC_CC += spec/arm_v7/virtualization/kernel/vm.cc
SRC_CC += spec/arm_v7/virtualization/kernel/cpu_context.cc
SRC_CC += spec/arm_v7/vm_session_component.cc
SRC_CC += spec/arm_v7/virtualization/vm_session_component.cc

# add assembly sources
SRC_S += spec/arm_v7/virtualization/mode_transition.s

# include less specific configuration
include $(REP_DIR)/lib/mk/exynos5/core.inc
