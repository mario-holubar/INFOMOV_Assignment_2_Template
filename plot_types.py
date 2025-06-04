import matplotlib.pyplot as plt
import numpy as np
import matplotlib.cm as cm

data = {
    'Associative': {
        'READ': [38143, 29914, 10036, 21907],
        'WRITE': [100000, 59728, 26780, 19332]
    },
    'Direct mapped': {
        'READ': [37475, 5890, 24757, 31878],
        'WRITE': [100000, 61895, 41731, 33713]
    },
    'N-way set associative': {
        'READ': [38116, 22730, 11774, 27380],
        'WRITE': [100000, 53221, 24577, 30885]
    }
}

cache_types = list(data.keys())
operations = ['READ', 'WRITE']
num_levels = 4
level_labels = ["L1", "L2", "L3", "DRAM"]

# Colormap: green (good) to red (bad)
cmap = cm.get_cmap('RdYlGn_r', num_levels)
colors = [cmap(i) for i in range(num_levels)]

x = np.arange(len(cache_types))
width = 0.5  # Wider bars for stacked chart

fig, axes = plt.subplots(1, 2, figsize=(14, 6))

for i, op in enumerate(operations):
    ax = axes[i]
    bottoms = np.zeros(len(cache_types))
    for lvl in range(num_levels):
        level_values = [data[ct][op][lvl] for ct in cache_types]
        ax.bar(x, level_values, width, bottom=bottoms, label=level_labels[lvl], color=colors[lvl])
        bottoms += level_values

    ax.set_title(f'{op} Operations')
    ax.set_xlabel('Cache Type')
    ax.set_xticks(x)
    ax.set_xticklabels(cache_types)
    ax.set_ylabel('Total cache hits')
    ax.legend(title='Cache Level')
    ax.grid(axis='y', linestyle='--', alpha=0.7)

fig.suptitle('Cache performance on spiral task by cache type', fontsize=16)
plt.tight_layout()
plt.show()
