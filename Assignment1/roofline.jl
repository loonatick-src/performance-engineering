using Plots
using NumericIO
# using InspectDR

struct RooflineData{F<:Real}
    pf::F
    pf_avx::F
    pf_avx_fma::F
    bw_l1::F
    bw_l2::F
    bw_l3::F
    bw_dram::F
end

peakflops_sp = 5.2 # GFlops
peakflops_sp_avx = 20.6 # GFlops
peakflops_sp_avx_fma = 41.2 # GFlops

bw_l1 = 39.180 # GB/s
bw_l2 = 20.274 # GB/s
bw_l3 = 18.407 # GB/s
bw_dram = 12.726 # GB/s

empirical_rl_data = RooflineData(peakflops_sp, peakflops_sp_avx, peakflops_sp_avx_fma, bw_l1, bw_l2, bw_l3, bw_dram)

roofline(ai, bw, pf) = min(ai * bw, pf)

# inspectdr()
pyplot()
# plotly()

function construct_roofline(data::RooflineData; ai_lo=-4, ai_hi=3)
    ais_ticks = 2.0 .^ (ai_lo:1:ai_hi)
    ais = 2.0 .^ LinRange(ai_lo, ai_hi, 100)
    # AVX2 FMA peakflops, L1 bandwidth
    p = plot(ais, roofline.(ais, data.bw_l1, data.pf_avx_fma); lc=:blue, linewidth=0.5, xlabel="Arithmetic Intensity (Flops/Byte)", ylabel="Acheived performance (GFLops)", xscale=:log2, yscale=:log2, legend=false)
    # L2 Bandwidth
    plot!(p, ais, roofline.(ais, data.bw_l2, data.pf_avx_fma); lc=:blue, linewidth=0.5)
    # L3 Bandwidth
    plot!(p, ais, roofline.(ais, data.bw_l3, data.pf_avx_fma); lc=:blue, linewidth=0.5)
    # DRAM bandwidth
    plot!(p, ais, roofline.(ais, data.bw_dram, data.pf_avx_fma); lc=:blue, linewidth=0.5)
    # AVX peakflops
    plot!(p, ais, roofline.(ais, data.bw_l1, data.pf_avx); lc=:blue, linewidth=0.5)
    # scalar peakflops
    plot!(p, ais, roofline.(ais, data.bw_l1, data.pf); lc=:blue, linewidth=0.5)
    xticks!(p, ais_ticks, string.(ais_ticks))
    yticks_iter = 2.0 .^ (0:6)
    yticks!(p, yticks_iter, string.(yticks_iter))
    return p
end

empirical_roofline_plot = construct_roofline(empirical_rl_data)


# requires `InspectDR.jl`, whose installation got kind of messed up. Troubleshoot after assignment
# empirical_roofline_plot.layout[:format_xtick] = TickLabelStyle(NumericIO.UEXPONENT_SI)