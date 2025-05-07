import matplotlib.pyplot as plt
import numpy as np

# Data (milliseconds)
methods = ['Sync', 'Async', 'Batch10', 'Batch100', 'Batch1000']
catchup_times_ms = [165.486, 26.444, 5.173, 3.451, 2.717]  # logging thread times in ms

# Plot setup
x = np.arange(len(methods))
width = 0.6  # wider bars since only one series

fig, ax = plt.subplots()
bars = ax.bar(x, catchup_times_ms, width, label='Catch-up Time', color='tab:orange')

# Labels and title
ax.set_xlabel('Persistence Method')
ax.set_ylabel('Catch-up Time (ms)')
ax.set_title('Logging Thread Catch-up Times for KDB+ Persistence Methods')
ax.set_xticks(x)
ax.set_xticklabels(methods)

# Annotate bars with milliseconds
for bar in bars:
    height = bar.get_height()
    ax.annotate(f'{height:.3f} ms',
                xy=(bar.get_x() + bar.get_width() / 2, height),
                xytext=(0, 3),
                textcoords='offset points',
                ha='center', va='bottom')

plt.tight_layout()
plt.show()