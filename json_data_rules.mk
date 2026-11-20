# JSON files are run through jsonproc, which is a tool that converts JSON data to an output file
# based on an Inja template. https://github.com/pantor/inja

AUTO_GEN_TARGETS += $(DATA_SRC_SUBDIR)/region_map/region_map_entries.h
$(DATA_SRC_SUBDIR)/region_map/region_map_entries.h: $(DATA_SRC_SUBDIR)/region_map/region_map_sections.json $(DATA_SRC_SUBDIR)/region_map/region_map_sections.json.txt
	$(JSONPROC) $^ $@

$(C_BUILDDIR)/region_map.o: c_dep += $(DATA_SRC_SUBDIR)/region_map/region_map_entries.h

AUTO_GEN_TARGETS += include/constants/region_map_sections.h
include/constants/region_map_sections.h: $(DATA_SRC_SUBDIR)/region_map/region_map_sections.json $(DATA_SRC_SUBDIR)/region_map/region_map_sections.constants.json.txt
	$(JSONPROC) $^ $@

# Hasep: `mapjson groups` (map_data_rules.mk) rewrites heal_locations.json in
# place (clean_heal_locations: truncate, then write). Nothing ordered the two,
# so under -j jsonproc could read it empty or half written ("unexpected end of
# input", "parse error at line 290": CI on master, 18-20/11). The order-only
# prerequisite on a file of that same rule makes jsonproc wait for it. It is
# not enough alone: the groups rule lists its targets with `:`, so under -j
# make may run it more than once. mapjson itself now rewrites the file only
# when it changes, and through a rename (tools/mapjson/mapjson.cpp).
HEAL_LOCATIONS_AFTER_MAPJSON := | $(INCLUDECONSTS_OUTDIR)/map_groups.h

AUTO_GEN_TARGETS += $(DATA_SRC_SUBDIR)/heal_locations.h
$(DATA_SRC_SUBDIR)/heal_locations.h: $(DATA_SRC_SUBDIR)/heal_locations.json $(DATA_SRC_SUBDIR)/heal_locations.json.txt $(HEAL_LOCATIONS_AFTER_MAPJSON)
	$(JSONPROC) $^ $@

$(C_BUILDDIR)/heal_location.o: c_dep += $(DATA_SRC_SUBDIR)/heal_locations.h

AUTO_GEN_TARGETS += include/constants/heal_locations.h
include/constants/heal_locations.h: $(DATA_SRC_SUBDIR)/heal_locations.json $(DATA_SRC_SUBDIR)/heal_locations.constants.json.txt $(HEAL_LOCATIONS_AFTER_MAPJSON)
	$(JSONPROC) $^ $@
