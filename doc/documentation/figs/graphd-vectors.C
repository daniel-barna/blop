main()
{
    plot(_1,_2,-_1,_2).ds(vectors().norm(8).use_color(true))
	.nsamples(6).p1range(-10,10).p2range(-10,10).legend("");

    blopeps::print("graphd-vectors.beps");
}
