CATAPULT_HOME  = /wv/hlsb/CATAPULT/10.5a/PRODUCTION/aol/Mgc_home
INCLUDES       = -I $(CATAPULT_HOME)/shared/include -I $(source_dir)
LIBRARIES      = -L$(CATAPULT_HOME)/shared/lib -lsystemc -lpthread
QUESTA_HOME    = /u/release/2020.1/questasim
VIS_HOME       = /u/release/2020.1/visualizer
VCS_HOME       = /wv/hlstools/vcs/O-2018.09-SP2-3

export PATH    := $(QUESTA_HOME)/bin:$(VIS_HOME)/bin:$(PATH)

SKIP_LIMITED_FEATURES_KEY = 1

GCC            = g++
LINK           = g++
CFLAGS         = -std=c++11 -g -O0

GCC_VERSION    ?= 5.3.0

VLOG           = $(QUESTA_HOME)/bin/vlog
VSIM           = $(QUESTA_HOME)/bin/vsim
VMAP           = $(QUESTA_HOME)/bin/vmap
VLIB           = $(QUESTA_HOME)/bin/vlib
VOPT           = $(QUESTA_HOME)/bin/vopt
SCCOM          = $(QUESTA_HOME)/bin/sccom

VLOG_FLAGS     = -work $(work) -cppinstall $(GCC_VERSION)
VIS_FLAGS      = -work $(work) -cppinstall $(GCC_VERSION) -t 1ps -visualizer -qwavedb=+signal+report+memory=1024+transaction+class
VSIM_FLAGS     = -work $(work) -cppinstall $(GCC_VERSION) -t 1ps 
VOPT_VIS_FLAGS = -undefsyms=verbose -debug +designfile -cppinstall $(GCC_VERSION)
VOPT_FLAGS     = -g -undefsyms=verbose -cppinstall $(GCC_VERSION)
SCCOM_FLAGS    = -g -O0 $(INCLUDES) -cppinstall $(GCC_VERSION) -std=c++11

DEFINES        = -D SC_INCLUDE_MTI_AC -D CONNECTIONS_ACCURATE_SIM -D SC_INCLUDE_DYNAMIC_PROCESSES

SYSCAN         = $(VCS_HOME)/bin/syscan
VLOGAN         = $(VCS_HOME)/bin/vlogan
VCS            = $(VCS_HOME)/bin/vcs

target         = pop_controller
vcs_target     = simv

marker_dir     = $(markers)/maker.mark
source_dir     = ./source_files
object_dir     = ./object_files

# markers

markers        = marker_files
build          = $(markers)/build.mark
build_vis      = $(markers)/build_vis.mark
object_lib     = $(markers)/object.mark
work_lib       = $(markers)/work.mark
tbench         = $(markers)/tbench.sv.mark
pop_controller = $(markers)/coin_counter_wrapper.sc.mark
link_marker    = $(markers)/sccom_link.mark
vcs_marker     = $(markers)/vcs.mark

design         = $(pop_controller) $(tbench)
opt_object     = tbench_opt
vis_opt_object = tbench_opt_vis

work           = ./work

VCS_CRUFT      = AN.DB csrc DVEfiles inter.vpd simv  simv.daidir ucli.key 
CRUFT          = transcript *.wlf modelsim.ini *stacktrace* qwave.db .visualizer visualizer.log design.bin core.*

#====== Questa target ==============

.PHONY: run_questa

run_questa: $(build)
	$(VSIM) $(VSIM_FLAGS) $(opt_object)

$(build): $(marker_dir) $(work_lib) $(link_marker)
	$(VOPT) $(VOPT_FLAGS) tbench -o $(opt_object)
	touch $(build)

$(link_marker): $(marker_dir) $(work_lib) $(design)
	$(SCCOM) -g -link -verbose -cppinstall $(GCC_VERSION)
	touch $(link_marker)

#====== Visualizer target ==========

.PHONY: run_vis

run_vis: $(build_vis)
	$(VSIM) $(VIS_FLAGS) $(vis_opt_object)

$(build_vis): $(marker_dir) $(work_lib) $(link_marker)
	$(VOPT) $(VOPT_VIS_FLAGS) tbench -o $(vis_opt_object)
	touch $(build_vis)

#====== G++ target ================

.PHONY: ddd_debug

ddd_debug: $(target)
	ddd $(target)

$(target): $(marker_dir) $(object_lib) $(object_dir)/$(target).o
	$(LINK) $(LD_FLAGS) -o $(target) $(object_dir)/$(target).o $(LIBRARIES)

#====== VCS target =================

run_vcs: $(vcs_target)
	./$(vcs_target) -gui

$(vcs_target): $(vcs_marker)

$(vcs_marker): $(marker_dir)
	$(SYSCAN) source_files/coin_counter_wrapper.cpp:coin_counter_wrapper -cflags "-g -O0 $(DEFINES) -D MATCHLIB -std=c++11 -I$(CATAPULT_HOME)/shared/include -I. -I./source_files/" 
	$(VLOGAN) source_files/tbench.sv -sverilog
	$(VCS) -sysc -debug -timescale=1ps/1ps tbench
	touch $@

#====== Generic compile rules ======
 
$(object_dir)/%.o: $(source_dir)/%.cpp
	$(GCC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(markers)/%.sv.mark: $(source_dir)/%.sv
	$(VLOG) $(VLOG_FLAGS) $<
	touch $@

$(markers)/%.sc.mark: $(source_dir)/%.cpp
	$(SCCOM) $(SCCOM_FLAGS) $(DEFINES) -D QUESTA -D MATCHLIB $<
	touch $@

#====== Header file dependencies ===

$(source_dir)/coin_counter_wrapper.cpp: $(source_dir)/coin_counter.h  $(source_dir)/coin_counter_mc.h  $(source_dir)/coin_counter_wrapper.h  $(source_dir)/ml_ac_channel.h  $(source_dir)/types.h
	rm -rf $(object_dir)/coin_counter_wrapper.o
	rm -rf $(markers)/coin_counter_wrapper.sc.mark
	touch $@

$(source_dir)/$(target).cpp: $(source_dir)/coin_counter.h $(source_dir)/types.h
	rm -rf $(object_dir)/$(target).o
	touch $@

#====== Working directories ========

$(marker_dir):
	mkdir $(markers)
	touch $(marker_dir)

$(object_lib):
	mkdir $(object_dir)
	touch $(object_lib)

$(work_lib): $(marker_dir)
	$(VLIB) $(work)
	$(VMAP) work $(work)
	touch $(work_lib)


#====== Clean up ===================

.PHONY: clean

clean:
	rm -rf $(target) $(work) $(markers) $(object_dir) $(CRUFT) $(VCS_CRUFT)

