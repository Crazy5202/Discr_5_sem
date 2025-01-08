import math
import random
import matplotlib.pyplot as plt
from shapely.geometry import Polygon
from shapely.geometry.point import Point


def generate_convex_polygon(num_vertices, x_range=(0, 1000), y_range=(0, 1000)):
    points = [(random.randint(*x_range), random.randint(*y_range)) for _ in range(num_vertices)]
    points = sorted(points, key=lambda p: (p[0], p[1]))  # Сортировка по x, затем по y

    def cross(o, a, b):
        return (a[0] - o[0]) * (b[1] - o[1]) - (a[1] - o[1]) * (b[0] - o[0])

    lower = []
    for p in points:
        while len(lower) >= 2 and cross(lower[-2], lower[-1], p) <= 0:
            lower.pop()
        lower.append(p)

    upper = []
    for p in reversed(points):
        while len(upper) >= 2 and cross(upper[-2], upper[-1], p) <= 0:
            upper.pop()
        upper.append(p)

    convex_hull = lower[:-1] + upper[:-1]

    centroid_x = sum(p[0] for p in convex_hull) / len(convex_hull)
    centroid_y = sum(p[1] for p in convex_hull) / len(convex_hull)
    centroid = (centroid_x, centroid_y)

    convex_hull.sort(key=lambda p: math.atan2(p[1] - centroid[1], p[0] - centroid[0]))
    return convex_hull

def polygons_intersect(new_polygon, existing_polygons):

    new_poly_shape = Polygon(new_polygon)
    for poly in existing_polygons:
        existing_poly_shape = Polygon(poly)
        if new_poly_shape.intersects(existing_poly_shape):
            return True
    return False

def generate_test_case(m, q, vertex_range=(3, 10), query_range=(0, 1000)):

    polygons = []
    for _ in range(m):
        while True:
            num_vertices = random.randint(*vertex_range)
            polygon = generate_convex_polygon(num_vertices)
            if not polygons_intersect(polygon, polygons):
                polygons.append(polygon)
                break

    queries = [(random.randint(query_range[0], query_range[1]),
                random.randint(query_range[0], query_range[1])) for _ in range(q)]

    input_data = [str(m)]
    for polygon in polygons:
        input_data.append(str(len(polygon)))
        for x, y in polygon:
            input_data.append(f"{x} {y}")

    input_data.append(str(q))
    for x, y in queries:
        input_data.append(f"{x} {y}")

    return "\n".join(input_data), polygons, queries

def save_test_cases(file_name, num_cases, m_range=(5, 10), q_range=(10, 30), vertex_range=(3, 8), query_range=(0, 1000)):

    with open(file_name, "w") as f:
        for _ in range(num_cases):
            m = random.randint(*m_range)
            q = random.randint(*q_range)
            test_case, polygons, queries = generate_test_case(m, q, vertex_range, query_range)
            f.write(test_case + "\n\n")
            visualize_test_case(_, polygons, queries)

            results = check_point_in_polygons(queries, polygons)
            for query, polygon_idx in results:
                if polygon_idx is not None:
                    f.write(str(polygon_idx))
                else:
                    f.write("-1")
                f.write("\n")
            f.write("\n")


def visualize_test_case(num, polygons, queries):
    plt.figure(figsize=(10, 10))

    for i, polygon in enumerate(polygons):
        x_coords = [point[0] for point in polygon] + [polygon[0][0]]
        y_coords = [point[1] for point in polygon] + [polygon[0][1]]
        plt.plot(x_coords, y_coords, marker='o', label=f'Polygon {i}')

    query_x = [q[0] for q in queries]
    query_y = [q[1] for q in queries]
    plt.scatter(query_x, query_y, color='red', label='Queries')

    for i, (x, y) in enumerate(queries):
        plt.text(x + 1, y + 1, str(i), color='red', fontsize=9)

    plt.axhline(0, color='black', linewidth=0.5)
    plt.axvline(0, color='black', linewidth=0.5)
    plt.grid(color='gray', linestyle='--', linewidth=0.5)
    plt.legend()
    plt.title("Polygon and Query Visualization " + str(num))
    plt.xlabel("X-axis")
    plt.ylabel("Y-axis")
    plt.savefig(f"case_{num}.png")

def check_point_in_polygons(queries, polygons):
    results = []
    for query in queries:
        point = Point(query)
        found = False
        for i, polygon in enumerate(polygons):
            poly_shape = Polygon(polygon)
            if poly_shape.contains(point):
                results.append((query, i))
                found = True
                break
        if not found:
            results.append((query, None))
    return results

if __name__ == "__main__":
    save_test_cases("test_cases.txt", num_cases=5)