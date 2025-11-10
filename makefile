# компилятор и флаги
CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -fPIC
LDFLAGS := -Wl,-rpath,'$$ORIGIN'

#цели
TARGET := encrypt_decrypt
SO_TARGETS := libconstants.so libutils.so libdiff.so libshamir.so libel_gamal.so
OBJS := main.o

.PHONY: all clean clear help test-libs run

#сборка проекта
all: $(SO_TARGETS) $(TARGET)
	@echo "=== Сборка завершена ==="
	@echo "Запустить проект: ./encrypt_decrypt"

#сборка главного файла
$(TARGET): $(OBJS) | $(SO_TARGETS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) -ldl -Wl,-rpath,'$$ORIGIN'

#сборка библиотек в определённом порядке
libconstants.so: constants.o
	$(CXX) -shared -o $@ $^ $(LDFLAGS)

libutils.so: utils.o | libconstants.so
	$(CXX) -shared -o $@ $^ -L. -lconstants $(LDFLAGS)

libdiff.so: diff.o | libutils.so libconstants.so
	$(CXX) -shared -o $@ $^ -L. -lutils -lconstants $(LDFLAGS)

libshamir.so: shamir.o | libutils.so libconstants.so
	$(CXX) -shared -o $@ $^ -L. -lutils -lconstants $(LDFLAGS)

libel_gamal.so: el_gamal.o | libutils.so libconstants.so
	$(CXX) -shared -o $@ $^ -L. -lutils -lconstants $(LDFLAGS)

#копиляция о-файлов
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

#зависимости
constants.o: constants.cpp constants.h
utils.o: utils.cpp utils.h
diff.o: diff.cpp diff.h utils.h constants.h
shamir.o: shamir.cpp shamir.h utils.h constants.h
el_gamal.o: el_gamal.cpp el_gamal.h utils.h constants.h
main.o: main.cpp

#очистка
clear:
	rm -f $(OBJS) constants.o utils.o diff.o shamir.o el_gamal.o
	rm -f $(TARGET) $(SO_TARGETS)
	rm -f diff_key.txt shamir_key.txt el_key.txt temp_input.txt *.bin
	@echo "=== Проект полностью очищен ==="

#помощь
help:
	@echo "Доступные команды:"
	@echo "make all       - сборка проекта и библиотек"
	@echo "make clear     - удаление всех сгенерированных файлов"
	@echo "make help      - показать эту справку"
