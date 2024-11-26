import matplotlib.pyplot as plt

# Данные
x = [5, 50, 200, 350, 500]
y = [33, 282, 3800, 12000,26793]

# Создаем график
plt.plot(x, y, marker='o', linestyle='-', color='b')

# Настройки осей и заголовок
plt.xlabel('n-арность "матрицы" единиц и нулей')
plt.ylabel('Время выполнения, мкс')

# Отображаем график
plt.show()