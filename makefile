# Компилятор и флаги
CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -fPIC
LDFLAGS := -Wl,-rpath,'$$ORIGIN'

# Цели
TARGET := encrypt_decrypt
SO_TARGETS := libutils.so libdiff.so libshamir.so libel_gamal.so
OBJS := main.o

.PHONY: all clean clear help test-libs run

# Основная цель - сборка всего
all: $(SO_TARGETS) $(TARGET)
	@echo "=== Сборка завершена ==="
	@echo "Запустить проект: ./encrypt_decrypt"

# Сборка главного исполняемого файла
$(TARGET): $(OBJS) | $(SO_TARGETS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) -L. -ldiff -lshamir -lel_gamal -lutils $(LDFLAGS)

# Сборка библиотек с правильными путями
libutils.so: utils.o
	$(CXX) -shared -o $@ $^ $(LDFLAGS)

libdiff.so: diff.o | libutils.so
	$(CXX) -shared -o $@ $^ -L. -lutils $(LDFLAGS)

libshamir.so: shamir.o | libutils.so
	$(CXX) -shared -o $@ $^ -L. -lutils $(LDFLAGS)

libel_gamal.so: el_gamal.o | libutils.so
	$(CXX) -shared -o $@ $^ -L. -lutils $(LDFLAGS)

# Компиляция объектных файлов
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Зависимости
diff.o: diff.cpp diff.h utils.h
el_gamal.o: el_gamal.cpp el_gamal.h utils.h
shamir.o: shamir.cpp shamir.h utils.h
utils.o: utils.cpp utils.h diff.h shamir.h el_gamal.h
main.o: main.cpp diff.h shamir.h el_gamal.h utils.h

# Тестовая цель для проверки библиотек - упрощенный вывод
test-libs: $(SO_TARGETS)
	@echo "=== Проверка библиотек ==="
	@for so in $(SO_TARGETS); do \
		echo -n "$$so: "; \
		if ldd $$so 2>/dev/null | grep -q "not found"; then \
			echo "не найден"; \
		else \
			echo "найден"; \
		fi; \
	done
	@echo "=== Проверка завершена ==="

# Полная очистка проекта
clear:
	rm -f $(OBJS) diff.o shamir.o el_gamal.o utils.o
	rm -f $(TARGET) $(SO_TARGETS)
	rm -f diff_key.txt shamir_key.txt el_key.txt temp_input.txt *.bin
	@echo "=== Проект полностью очищен ==="

# Помощь
help:
	@echo "Доступные команды:"
	@echo "make all       - сборка проекта и библиотек"
	@echo "make test-libs - проверка созданных библиотек"
	@echo "make clear     - удаление всех сгенерированных файлов"
	@echo "make help      - показать эту справку"
