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

.PHONY: dirs clean jobs grade

dirs:
		/bin/mkdir -p $(OBJDIR)/ $(APPSOBJDIR)

clean:
		/bin/rm -rf $(OBJDIR) *~ $(APP_NAME) jobs/$(USER)_*.job

OBJS=$(OBJDIR)/main.o $(OBJDIR)/parse_args.o $(OBJDIR)/ts_hashtable.o $(OBJDIR)/util.o $(OBJDIR)/graph.o $(OBJDIR)/vertex_set.o $(APPSOBJDIR)/bfs.o $(APPSOBJDIR)/page_rank.o $(APPSOBJDIR)/graph_decomposition.o $(APPSOBJDIR)/kBFS.o

# Generate job for specific app and graph
jobs: $(APP_NAME)
		cd jobs && ./generate_job.sh $(APP) $(GRAPH)

# Generate job for grading
grade: $(APP_NAME)
		cd jobs && ./generate_grade_performance_job.sh

$(APP_NAME): dirs $(OBJS) $(REFOBJS)
		$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(REFDIR)/phi_obj/*

cpu: dirs $(OBJS) $(REFOBJS)
		$(CXX) $(CXXFLAGS) -o $(APP_NAME) $(OBJS) $(REFDIR)/cpu_obj/*

$(APPSOBJDIR)/%.o: apps/%.cpp paraGraph.h
		$(CXX) $< $(CXXFLAGS) -c -o $@

$(OBJDIR)/%.o: %.cpp
		$(CXX) $< $(CXXFLAGS) -c -o $@

$(OBJDIR)/main.o: CycleTimer.h grade.h

cpu_run: cpu
	./paraGraph bfs ./graph/com-youtube_3m.graph; ./paraGraph kbfs ./graph/com-youtube_3m.graph; ./paraGraph pagerank ./graph/com-youtube_3m.graph;

cpu_run_ghc: cpu
	./paraGraph bfs /afs/cs/academic/class/15418-s16/public/asst3_graphs/soc-livejournal1_68m.graph; ./paraGraph kbfs /afs/cs/academic/class/15418-s16/public/asst3_graphs/soc-livejournal1_68m.graph; ./paraGraph pagerank /afs/cs/academic/class/15418-s16/public/asst3_graphs/soc-livejournal1_68m.graph;

cpu_run_pr: cpu
	./paraGraph pagerank /afs/cs/academic/class/15418-s16/public/asst3_graphs/soc-livejournal1_68m.graph;

cpu_run_soc: cpu
	./paraGraph bfs ./graph/soc-slashdot_900k.graph; ./paraGraph kbfs ./graph/soc-slashdot_900k.graph; ./paraGraph pagerank ./graph/soc-slashdot_900k.graph;

cpu_tiny: cpu
	./paraGraph bfs ./graph/tiny.graph

handin:
	tar -cf handin.tar writeup.pdf paraGraph.h vertex_set.h vertex_set.cpp apps/*.cpp

