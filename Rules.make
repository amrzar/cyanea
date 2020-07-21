
 #
 # Copyright 2020 Amirreza Zarrabi <amrzar@gmail.com>
 #
 # This program is free software; you can redistribute it and/or modify
 # it under the terms of the GNU General Public License as published by
 # the Free Software Foundation; either version 2 of the License, or
 # (at your option) any later version.
 #
 # This program is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU General Public License for more details.
 #

subdirs = $(sort $(filter %/,$(objects)))

 # ... object's depfiles.
-include $(patsubst %.o,%.d,$(filter-out %/,$(objects)))
 
%.o: %.c FORCE
	$(Q)echo "CC $<"
	$(Q)$(CC) -MMD $(NOSTDINC_FLAGS) $(CPPFLAGS) $(CFLAGS) \
		$(cc-flags) $(cc-flags-$(@F)) -c -o $@ $<

%.o: %.S FORCE
	$(Q)echo "CC $<"
	$(Q)$(CC) -MMD $(NOSTDINC_FLAGS) $(CPPFLAGS) $(AFLAGS) \
		$(as-flags) $(as-flags-$(@F)) -c -o $@ $<

.output.o: $(filter-out %/,$(objects)) $(subdirs)
	$(Q)echo "LD $@"
	$(Q)$(LD) $(LDFLAGS) $(ld-flags) \
		$(patsubst %/,%/.output.o,$(objects)) -r -o $@

$(subdirs): FORCE
 # Descending ...
	$(Q)$(MAKE) -C $@

FORCE:
.PHONY: FORCE
