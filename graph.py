import matplotlib.pyplot as plt

# Данные
x = [100, 1000, 10000, 100000]
y = [1000, 1448, 18387, 211960]

# Создаем график
plt.plot(x, y, marker='o', linestyle='-', color='b')

# Настройки осей и заголовок
plt.xlabel('Число рёбер и вершин')
plt.ylabel('Время выполнения, мкс')

# Отображаем график
plt.show()