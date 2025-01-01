import matplotlib.pyplot as plt

# Данные
x = [100, 1000, 10000, 100000]
y = [1, 6, 91, 1352]

# Создаем график
plt.plot(x, y, marker='o', linestyle='-', color='b')

# Настройки осей и заголовок
plt.xlabel('Число точек и отрезков')
plt.ylabel('Время выполнения, мс')

# Отображаем график
plt.show()