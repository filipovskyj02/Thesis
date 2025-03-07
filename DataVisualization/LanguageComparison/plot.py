import matplotlib.pyplot as plt

# Benchmark data: (label, Python time, Java time, C++ time)
benchmarks = [
    ("1M_Init_2M_Orders_67%_Limit_95%_Cancel", 5945, 963, 721),
    ("500K_Init_1M_Orders_67%_Limit_50%_Cancel", 4125, 513, 370),
    ("30K_Init_500K_Orders_20%_Limit_0%_Cancel", 1066, 148, 123),
]

# Plot and save each benchmark separately
for label, py_time, java_time, cpp_time in benchmarks:
    plt.figure(figsize=(6, 4))
    bar_width = 0.5
    languages = ["Python 3.13", "Java 21", "C++ (GCC 14.2)"]
    times = [py_time, java_time, cpp_time]
    colors = ["yellow", "orange", "blue"]

    bars = plt.bar(languages, times, color=colors, width=bar_width)

    # Annotate each bar with its value
    for bar in bars:
        plt.text(bar.get_x() + bar.get_width() / 2, bar.get_height() * 1.05,
                 f"{int(bar.get_height())} ms", ha="center", fontsize=12, fontweight="bold")

    # Labels and title
    plt.xlabel("Programming Language")
    plt.ylabel("Execution Time (ms)")
    plt.title(f"Benchmark: {label.replace('_', ' ')}\n(Average over 5 runs)")

    # Adjust Y-axis to prevent numbers from touching the graph border
    plt.ylim(0, max(times) * 1.2)

    # Save the plot
    filename = f"{label}.png"
    plt.savefig(filename, dpi=300, bbox_inches="tight")
    plt.close()

    print(f"Saved: {filename}")


