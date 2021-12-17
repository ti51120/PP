CXX=g++ -m64
CXXFLAGS=-I./common -Iobjs/ -O3 -std=c++17 -Wall

APP_NAME=canny
OBJDIR=objs
COMMONDIR=./common

PPM_CXX=$(COMMONDIR)/bmpfuncs.cpp
PPM_OBJ=$(addprefix $(OBJDIR)/, $(subst $(COMMONDIR)/,, $(PPM_CXX:.cpp=.o)))


default: $(APP_NAME)

.PHONY: dirs clean

dirs:
		/bin/mkdir -p $(OBJDIR)/

clean:
		/bin/rm -rf $(OBJDIR) *.bmp *~ $(APP_NAME)

OBJS=$(OBJDIR)/main.o $(OBJDIR)/canny_serial.o $(OBJDIR)/canny_thread.o $(PPM_OBJ)

$(APP_NAME): dirs $(OBJS)
		$(CXX) $(CXXFLAGS) -o $@ $(OBJS) -lm -lpthread

$(OBJDIR)/%.o: %.cpp
		$(CXX) $< $(CXXFLAGS) -c -o $@

$(OBJDIR)/%.o: $(COMMONDIR)/%.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@

$(OBJDIR)/main.o: $(COMMONDIR)/CycleTimer.h

