import matplotlib.pyplot as plt

# Данные
x = [100000, 200000, 300000, 400000, 500000]
y = [2861, 5537, 8785, 12388, 15335]

# Создаем график
plt.plot(x, y, marker='o', linestyle='-', color='b')

# Настройки осей и заголовок
plt.xlabel('Число рёбер и запросов')
plt.ylabel('Время выполнения, мс')

# Отображаем график
plt.show()