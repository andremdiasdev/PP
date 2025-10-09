# ### Processor Characteristics
# Frequency: 2.6 GHz (Turbo Boost across all cores)
# Cores: 28 physical cores
# Hyperthreads: 2 per core → 56 threads
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
NoOfSockets = 2
# Theoretical Maximum FLOPs
TheoreticalMaximumFlops = ProcessorCores * Hyperthreads * ProcessorFrequency * VectorWidth / WordSizeBits * FMA
print("Theoretical Maximum Flops =", TheoreticalMaximumFlops, "GFLOPS/s")
# Theoretical Memory Bandwidth
TheoreticalMemoryBandwidth = DataTransferRate * MemoryChannels * BytesTransferredPerAccess * NoOfSockets/ 1000
print("Theoretical Maximum Bandwidth (at main memory) =", TheoreticalMemoryBandwidth, "GiB/s")