import matplotlib.pyplot as plt

# Benchmark results (averaged over 5 runs)
versions = ["Python 3.10.12", "Python 3.13"]
times = [7687, 5945]  # Execution times in milliseconds
colors = ["red", "green"]

# Create bar plot
plt.figure(figsize=(6, 5))
bars = plt.bar(versions, times, color=colors)

# Annotate bars with execution times on top
for bar, time in zip(bars, times):
    plt.text(bar.get_x() + bar.get_width() / 2, bar.get_height() + 200, f"{time} ms", 
             ha="center", va="bottom", fontsize=12, fontweight="bold")

# Labels and title
plt.ylabel("Execution Time (ms)")
plt.title("Benchmark: 1M_Init_2M_Orders_67%_Limit_95%_Cancel")
plt.ylim(0, 10000)  # Start scale from 0

# Save the figure
plt.savefig("python_version_comparison.png", dpi=300)
plt.close()
