#!/bin/bash
tmux set -g mouse on 
tmux new-window -n softsim
tmux new-window -n trxs
tmux new-window -n Evil-nitb
tmux new-window -n Evil-bts
tmux new-window -n Orange-trxcon
tmux new-window -n Orange-MS
tmux send-keys -t "softsim" '/softsim/src/demo_server.rb --type pcsc --socket unix --unix /tmp/osmocom_sap' Enter
tmux send-keys -t "trxs" '/heartbreaker/osmocom-bb/src/target/trx_toolkit/fake_trx.py' Enter
sleep 1
tmux send-keys -t "Evil-nitb" '/heartbreaker/nitb.sh' Enter
tmux send-keys -t "Evil-bts" 'osmo-bts-trx -c /heartbreaker/evil-bts.cfg' Enter
tmux send-keys -t "Orange-trxcon" '/heartbreaker/osmocom-bb/src/host/trxcon/trxcon' Enter
sleep 1
tmux send-keys -t "Orange-MS" '/heartbreaker/osmocom-bb/src/host/layer23/src/mobile/mobile' Enter

tmux select-window -t "Evil-nitb"
