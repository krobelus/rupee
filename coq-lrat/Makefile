compile: Map.vo Fset.vo Mapaxioms.vo Mapiter.vo Mapsubset.vo Lsort.vo Mapfold.vo Mapcard.vo Mapcanon.vo Mapc.vo Maplists.vo Adalloc.vo Allmaps.vo Basic.vo BinaryTrees.vo SetClause.vo SetICNF.vo SetChecker.vo
# 	rm -fv *.o
# 	coqc ExtractionOcaml.v
# 	rm -rf _build Interface.native
# 	ocamlbuild -use-ocamlfind Interface.native
# 	rm Interface.native
# 	cp _build/Interface.native .
# 	./Interface.native example1.cnf example1.lrat
clean:
	rm -fv *.vo *.o *.glob .*.aux Interface Interface.native Checker.ml Checker.mli
	rm -frv _build
%.vo: %.v
	coqc $<
