APP_NAME=paraGraph
OBJDIR=objs
APPSDIR=apps
APPSOBJDIR=$(OBJDIR)/$(APPSDIR)

REFDIR=ref

default: $(APP_NAME)

# Compile for Phi
$(APP_NAME): CXX = icc -m64 -std=c++11
$(APP_NAME): CXXFLAGS = -I. -O3 -Wall -openmp -offload-attribute-target=mic -DRUN_MIC

# Compile for CPU
cpu: CXX = g++ -m64 -std=c++11
cpu: CXXFLAGS = -I. -O3 -Wall -fopenmp -Wno-unknown-pragmas

.PHONY: dirs clean jobs

dirs:
		/bin/mkdir -p $(OBJDIR)/ $(APPSOBJDIR)

clean:
		/bin/rm -rf $(OBJDIR) *~ $(APP_NAME) jobs/$(USER)_*.job

OBJS=$(OBJDIR)/main.o $(OBJDIR)/parse_args.o $(OBJDIR)/graph.o $(OBJDIR)/vertex_set.o $(APPSOBJDIR)/bfs.o $(APPSOBJDIR)/page_rank.o $(APPSOBJDIR)/graph_decomposition.o $(APPSOBJDIR)/kBFS.o

#REFOBJS=$(REFDIR)/$(APPSDIR)/bfs_ref.o $(REFDIR)/$(APPSDIR)/page_rank_ref.o $(REFDIR)/$(APPSDIR)/graph_decomposition_ref.o $(REFDIR)/$(APPSDIR)/kBFS_ref.o $(REFDIR)/util_ref.o $(REFDIR)/vertex_set_ref.o

jobs: $(APP_NAME)
		cd jobs && ./generate_job.sh $(APP) $(GRAPH)

$(APP_NAME): dirs $(OBJS) $(REFOBJS)
		$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(REFDIR)/phi_obj/*

cpu: dirs $(OBJS) $(REFOBJS)
		$(CXX) $(CXXFLAGS) -o $(APP_NAME) $(OBJS) $(REFDIR)/cpu_obj/*

$(APPSOBJDIR)/%.o: apps/%.cpp paraGraph.h
		$(CXX) $< $(CXXFLAGS) -c -o $@

$(OBJDIR)/%.o: %.cpp
		$(CXX) $< $(CXXFLAGS) -c -o $@

$(OBJDIR)/main.o: CycleTimer.h grade.h
