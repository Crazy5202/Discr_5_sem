import matplotlib.pyplot as plt
import sys

def main():
    # Use sys.stdin for input to allow file redirection
    input_stream = sys.stdin

    num_segments, num_points = map(int, input_stream.readline().strip().split())
    segments = []
    points = []
    
    # Get segment points
    for _ in range(num_segments):
        x1, x2, y = map(int, input_stream.readline().strip().split())
        segments.append(((x1, y), (x2, y)))
    
    # Get point coordinates
    for _ in range(num_points):
        x, y = map(int, input_stream.readline().strip().split())
        points.append((x, y))
    
    # Plotting
    plt.figure()
    
    # Draw segments
    for ((x1, y1), (x2, y2)) in segments:
        plt.plot([x1, x2], [y1, y2], 'b', marker='o')
    
    # Draw points
    for (x, y) in points:
        plt.plot(x, y, 'ro')  # 'ro' means red circle
    
    # Customize and show the plot
    plt.title("Segments and Points")
    plt.xlabel("X-axis")
    plt.ylabel("Y-axis")
    plt.grid(True)
    plt.axis('equal')
    plt.savefig("drawn.png")

if __name__ == "__main__":
    main()