{
  "targets": [
    {
      "target_name": "binding",
      "defines": ["PRINT_ENABLED=1"],
      "sources": [
        "src/module.cc",
        "src/functions.cc",
        "src/emitter.cc",
        "src/jtocconversion.cc",
        "src/ctojconversion.cc",
        "src/s_rp.c",
        "src/init.c",
        "src/device.c",
        "src/listen.c",
        "src/h_iam.c",
        "src/h_whois.c",
        "src/h_rp.c",
        "src/h_rp_a.c",
        "src/h_errors.c",
        "bacnet-stack/src/apdu.c",
        "bacnet-stack/src/npdu.c",
        "bacnet-stack/src/bacdcode.c",
        "bacnet-stack/src/bacint.c",
        "bacnet-stack/src/bacreal.c",
        "bacnet-stack/src/bacstr.c",
        "bacnet-stack/src/bacapp.c",
        "bacnet-stack/src/bacprop.c",
        "bacnet-stack/src/bactext.c",
        "bacnet-stack/src/bip.c",
        "bacnet-stack/src/bvlc.c",
        "bacnet-stack/src/datetime.c",
        "bacnet-stack/src/indtext.c",
        "bacnet-stack/src/key.c",
        "bacnet-stack/src/keylist.c",
        "bacnet-stack/src/proplist.c",
        "bacnet-stack/src/debug.c",
        "bacnet-stack/src/bigend.c",
        "bacnet-stack/src/arf.c",
        "bacnet-stack/src/awf.c",
        "bacnet-stack/src/cov.c",
        "bacnet-stack/src/dcc.c",
        "bacnet-stack/src/iam.c",
        "bacnet-stack/src/ihave.c",
        "bacnet-stack/src/rd.c",
        "bacnet-stack/src/rp.c",
        "bacnet-stack/src/rpm.c",
        "bacnet-stack/src/timesync.c",
        "bacnet-stack/src/whohas.c",
        "bacnet-stack/src/whois.c",
        "bacnet-stack/src/wp.c",
        "bacnet-stack/src/wpm.c",
        "bacnet-stack/src/abort.c",
        "bacnet-stack/src/reject.c",
        "bacnet-stack/src/bacerror.c",
        "bacnet-stack/src/ptransfer.c",
        "bacnet-stack/src/memcopy.c",
        "bacnet-stack/src/filename.c",
        "bacnet-stack/src/tsm.c",
        "bacnet-stack/src/bacaddr.c",
        "bacnet-stack/src/address.c",
        "bacnet-stack/src/bacdevobjpropref.c",
        "bacnet-stack/src/bacpropstates.c",
        "bacnet-stack/src/alarm_ack.c",
        "bacnet-stack/src/event.c",
        "bacnet-stack/src/getevent.c",
        "bacnet-stack/src/get_alarm_sum.c",
        "bacnet-stack/src/readrange.c",
        "bacnet-stack/src/timestamp.c",
        "bacnet-stack/src/lighting.c",
        "bacnet-stack/src/version.c",
        "bacnet-stack/demo/handler/h_npdu.c",
        "bacnet-stack/demo/handler/s_whois.c",
        "bacnet-stack/demo/handler/s_iam.c",
        "bacnet-stack/demo/handler/noserv.c",
        "bacnet-stack/demo/handler/txbuf.c"
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "include",
        "bacnet-stack/include",
        "bacnet-stack/demo/handler"
      ],
      "conditions": [
        [
          "OS=='mac'",
          {
            "sources": [
              "bacnet-stack/ports/bsd/bip-init.c"
            ],
            "include_dirs": [
              "bacnet-stack/ports/bsd"
            ]
          }
        ],
        [
          "OS=='linux'",
          {
            "sources": [
              "bacnet-stack/ports/linux/bip-init.c"
            ],
            "include_dirs": [
              "bacnet-stack/ports/linux"
            ]
          }
        ]
      ]
    }
  ]
}