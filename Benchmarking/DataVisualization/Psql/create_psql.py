import matplotlib.pyplot as plt
import numpy as np

# Data (seconds)
methods = ['Sync', 'Offload', 'Batch100', 'Batch1000', 'COPY1000']
engine_times = [4.177, 0.00219, 0.00219, 0.00226, 0.002187]
catchup_times = [np.nan, 4.154, 0.084731, 0.04337, 0.02519]  # Hide Sync catch-up

# Plot setup
x = np.arange(len(methods))
width = 0.35

fig, ax = plt.subplots()
bar1 = ax.bar(x - width/2, engine_times, width, label='Engine Time')
bar2 = ax.bar(x + width/2, catchup_times, width, label='Catch-up Time')

# Set log scale on y-axis and start at 1e-3
ax.set_yscale('log')
ax.set_ylim(1e-3, None)

# Labels and title
ax.set_xlabel('Persistence Method')
ax.set_ylabel(r'Time (s, $\mathbf{log\ scale}$)')
ax.set_title('Core Engine vs. Catch-up Times by Persistence Method')
ax.set_xticks(x)
ax.set_xticklabels(methods)
ax.legend()

# Annotate bars with seconds
def annotate_bars(bars):
    for bar in bars:
        height = bar.get_height()
        if not np.isfinite(height):
            continue
        ax.annotate(f'{height:.3f}s',
                    xy=(bar.get_x() + bar.get_width() / 2, height),
                    xytext=(0, 3),
                    textcoords='offset points',
                    ha='center', va='bottom')

annotate_bars(bar1)
annotate_bars(bar2)

plt.tight_layout()
plt.show()