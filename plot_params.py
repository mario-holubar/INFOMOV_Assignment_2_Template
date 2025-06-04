import matplotlib.pyplot as plt
import numpy as np
import matplotlib.cm as cm

spiral = {
    'N=1,\nL1=4KB,\nL2=64KB,\nL3=256KB': {
        'READ': [37475, 5890, 24757, 31878],
        'WRITE': [100000, 61895, 41731, 33713]
    },
    'N=64,\nL1=4KB,\nL2=64KB,\nL3=256KB': {
        'READ': [38116, 22730, 11774, 27380],
        'WRITE': [100000, 53221, 24577, 30885]
    },
    'N=64,\nL1=64KB,\nL2=256KB,\nL3=1MB': {
        'READ': [59872, 12063, 7812, 20253],
        'WRITE': [100000, 22789, 27361, 12892]
    }
}
fractal = {
    'N=1,\nL1=4KB,\nL2=64KB,\nL3=256KB': {
        'READ': [55792051, 426465, 748820, 9172274],
        'WRITE': [33069805, 9165356, 5990563, 9382763]
    },
    'N=64,\nL1=4KB,\nL2=64KB,\nL3=256KB': {
        'READ': [55843242, 436268, 697965, 9162135],
        'WRITE': [33069805, 9644845, 7334440, 9348214]
    },
    'N=64,\nL1=64KB,\nL2=256KB,\nL3=1MB': {
        'READ': [56264972, 714938, 2669054, 6490646],
        'WRITE': [33069805, 7338476, 8122615, 7058478]
    }
}
ant = {
    'N=1,\nL1=4KB,\nL2=64KB,\nL3=256KB': {
        'READ': [146329, 50445, 277, 2949],
        'WRITE': [100000, 53607, 2092, 351]
    },
    'N=64,\nL1=4KB,\nL2=64KB,\nL3=256KB': {
        'READ': [196497, 545, 9, 2949],
        'WRITE': [100000, 2868, 1567, 458]
    },
    'N=64,\nL1=64KB,\nL2=256KB,\nL3=1MB': {
        'READ': [196814, 234, 3, 2949],
        'WRITE': [100000, 1807, 484, 25]
    }
}

task = "Spiral"
data = spiral

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
    #ax.set_xlabel('Parameters')
    ax.set_xticks(x)
    ax.set_xticklabels(cache_types)
    ax.set_ylabel('Total cache hits')
    ax.legend(title='Cache Level')
    ax.grid(axis='y', linestyle='--', alpha=0.7)

fig.suptitle(f'Cache performance by parameters on {task} task', fontsize=16)
plt.tight_layout()
plt.show()
