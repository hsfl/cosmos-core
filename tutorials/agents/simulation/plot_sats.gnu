
set terminal gif animate delay 1
set output "new_orbit_ijk.gif"
stats "new_orbit_ijk" nooutput


do for [i=0:int(STATS_blocks)-1] {
	print i
	set view equal xyz
	splot [-10000000:10000000][-10000000:10000000][-10000000:10000000] "new_orbit_ijk" index i, "new_orbit_ijk" index (i+100)%int(STATS_blocks), "new_orbit_ijk" index (i+200)%int(STATS_blocks), "new_orbit_ijk" index (i+300)%int(STATS_blocks), "new_orbit_ijk" index (i+400)%int(STATS_blocks)

}
