#компилятор и флаги
CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -fPIC -O2
LDFLAGS := -Wl,-rpath,'$$ORIGIN'
LIBFLAGS := -shared

#цели
TARGET := encrypt_decrypt
SO_TARGETS := libscytale.so libshamir.so libpolybius.so
OBJS := main.o utils.o

.PHONY: all clear help

#сборка проекта
all: $(SO_TARGETS) $(TARGET)
	@echo "=== Сборка завершена ==="
	@echo "Запустить проект: ./$(TARGET)"

#сборка главного файла
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) -ldl $(LDFLAGS)

#правила для динамических библиотек
libscytale.so: scytale.o utils.o
	$(CXX) $(LIBFLAGS) -o $@ $^ $(LDFLAGS)

libshamir.so: shamir.o utils.o
	$(CXX) $(LIBFLAGS) -o $@ $^ $(LDFLAGS)

libpolybius.so: polybius.o utils.o
	$(CXX) $(LIBFLAGS) -o $@ $^ $(LDFLAGS)

#компиляция объектных файлов
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

#очистка
clear:
	rm -f $(OBJS) scytale.o shamir.o polybius.o
	rm -f $(TARGET) $(SO_TARGETS)
	rm -f scytale_key.txt shamir_key.txt polybius_key.txt temp.txt *.bin *.enc *.dec
	@echo "=== Проект полностью очищен ==="

#помощь
help:
	@echo "Доступные команды:"
	@echo "make all - сборка проекта и библиотек"
	@echo "make clear - полная очистка проекта"
	@echo "make help - показать эту справку"
