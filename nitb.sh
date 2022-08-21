#!/bin/bash
exec osmo-nitb -c /heartbreaker/openbsc.cfg -l /heartbreaker/hlr.sqlite3 -P -M /tmp/bsc_mncc -C --debug=DRLL:DCC:DMM:DRR:DRSL:DNM
