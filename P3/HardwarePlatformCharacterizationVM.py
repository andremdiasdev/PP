#!/usr/bin/env python
# coding: utf-8

# ## Hardware Platform Characterization

# CPU and Memory Descriptions
import numpy as np
CPUDescription = "Intel(R) Xeon(R) CPU E5-2690 v4"
MemoryDescription = "M393A4K40BB1-CRC-2400"
print(CPUDescription)
print(MemoryDescription)

# ### Processor Characteristics
# Frequency: 3.7 GHz (Turbo Boost across all cores)
# Cores: 6 physical cores
# Hyperthreads: 2 per core → 12 threads
# Vector Width: 256 bits
# Word Size: 64 bits (double precision)
# FMA: 2 (Fused Multiply Add)

ProcessorFrequency = 2.6
ProcessorCores = 28
Hyperthreads = 2
VectorWidth = 256
WordSizeBits = 64
FMA = 2

# ### Main Memory Characteristics
# Data Transfer Rate: 2400 MT/s
# Channels: 2
# Bytes Transferred per Access: 8 bytes

DataTransferRate = 2400
MemoryChannels = 4
BytesTransferredPerAccess = 8
sockets = 2
# Theoretical Maximum FLOPs
TheoreticalMaximumFlops = ProcessorCores * Hyperthreads * ProcessorFrequency * VectorWidth / WordSizeBits * FMA
print("Theoretical Maximum Flops =", TheoreticalMaximumFlops, "GFLOPS/s")

# Theoretical Memory Bandwidth
TheoreticalMemoryBandwidth = DataTransferRate * MemoryChannels * BytesTransferredPerAccess* sockets  / 1000
print("Theoretical Maximum Bandwidth (at main memory) =", TheoreticalMemoryBandwidth, "GiB/s")

# Machine Balance
WordSizeBytes = WordSizeBits / 8
TheoreticalMachineBalance = TheoreticalMaximumFlops / TheoreticalMemoryBandwidth
print("Theoretical Machine Balance = ", TheoreticalMachineBalance, "Flops/byte")
print("Theoretical Machine Balance = ", TheoreticalMachineBalance * WordSizeBytes, "Flops/word")

# Roofline Model Data
smemroofs = [141.6, 95.1, 66.5, 37.6, 31.0, 29.3]
scomproofs = [8.9]
smem_roof_name = ["L1 Bandwidth", "L2 Bandwidth", "L3 Bandwidth", "L4 Bandwidth", "L5 Bandwidth", "DRAM Bandwidth"]
scomp_roof_name = ["FP64 Maximum"]
AI = [0.1163, 0.1760]
FLOPS = [14.31, 2.333]
labels = ["CloverLeaf w/OpenMP and Vectorization", "CloverLeaf Serial"]

print('memroofs', smemroofs)
print('mem_roof_names', smem_roof_name)
print('comproofs', scomproofs)
print('comp_roof_names', scomp_roof_name)
print('AI', AI)
print('FLOPS', FLOPS)
print('labels', labels)

# Plotting Roofline
import numpy as np
import matplotlib.pyplot as plt

font = {'size': 20}
plt.rc('font', **font)

markersize = 16
colors = ['b', 'g', 'r', 'y', 'm', 'c']
styles = ['o', 's', 'v', '^', 'D', ">", "<", "*", "h", "H", "+", "1", "2", "3", "4", "8", "p", "d", "|", "_", ".", ","]

fig = plt.figure(1, figsize=(20.67, 12.6))
plt.clf()
ax = fig.gca()
ax.set_xscale('log')
ax.set_yscale('log')
ax.set_xlabel('Arithmetic Intensity [FLOPs/Byte]')
ax.set_ylabel('Performance [GFLOP/sec]')
ax.grid()
ax.grid(which='minor', linestyle=':', linewidth=0.5, color='black')

nx = 10000
xmin = -3
xmax = 2
ymin = 0.1
ymax = 10000

ax.set_xlim(10**xmin, 10**xmax)
ax.set_ylim(ymin, ymax)

ixx = int(nx * 0.02)
xlim = ax.get_xlim()
ylim = ax.get_ylim()

scomp_x_elbow = []
scomp_ix_elbow = []
smem_x_elbow = []
smem_ix_elbow = []

x = np.logspace(xmin, xmax, nx)
for roof in scomproofs:
    for ix in range(1, nx):
        if smemroofs[0] * x[ix] >= roof and smemroofs[0] * x[ix - 1] < roof:
            scomp_x_elbow.append(x[ix - 1])
            scomp_ix_elbow.append(ix - 1)
            break

for roof in smemroofs:
    for ix in range(1, nx):
        if (scomproofs[0] <= roof * x[ix] and scomproofs[0] > roof * x[ix - 1]):
            smem_x_elbow.append(x[ix - 1])
            smem_ix_elbow.append(ix - 1)
            break

for i in range(0, len(scomproofs)):
    y = np.ones(len(x)) * scomproofs[i]
    ax.plot(x[scomp_ix_elbow[i]:], y[scomp_ix_elbow[i]:], c='k', ls='-', lw='2')

for i in range(0, len(smemroofs)):
    y = x * smemroofs[i]
    ax.plot(x[:smem_ix_elbow[i]+1], y[:smem_ix_elbow[i]+1], c='k', ls='-', lw='2')

marker_handles = list()
for i in range(0, len(AI)):
    ax.plot(float(AI[i]), float(FLOPS[i]), c=colors[i], marker=styles[i], linestyle='None', ms=markersize, label=labels[i])
    marker_handles.append(ax.plot([], [], c=colors[i], marker=styles[i], linestyle='None', ms=markersize, label=labels[i])[0])

for roof in scomproofs:
    ax.text(x[-ixx], roof,
            scomp_roof_name[scomproofs.index(roof)] + ': ' + '{0:.1f}'.format(float(roof)) + ' GFLOP/s',
            horizontalalignment='right',
            verticalalignment='bottom')

for roof in smemroofs:
    ang = np.arctan(np.log10(xlim[1] / xlim[0]) / np.log10(ylim[1] / ylim[0])
                    * fig.get_size_inches()[1] / fig.get_size_inches()[0])
    ax.text(x[ixx], x[ixx] * roof * (1 + 0.25 * np.sin(ang) ** 2),
            smem_roof_name[smemroofs.index(roof)] + ': ' + '{0:.1f}'.format(float(roof)) + ' GiB/s',
            horizontalalignment='left',
            verticalalignment='bottom',
            rotation=180 / np.pi * ang)

leg1 = plt.legend(handles=marker_handles, loc=4, ncol=2)
ax.add_artist(leg1)

plt.savefig('roofline.png')
plt.savefig('roofline.eps')
plt.savefig('roofline.pdf')
plt.savefig('roofline.svg')

plt.show()


 
