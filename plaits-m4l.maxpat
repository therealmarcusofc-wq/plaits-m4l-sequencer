{
  "boxes" : [ {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.thisdevice",
      "numinlets" : 0,
      "numoutlets" : 1,
      "outlettype" : [ "object" ],
      "patching_rect" : [ 10, 10, 100, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.menu @items Major Minor Dorian Phrygian Lydian Mixolydian Pentatonic Chromatic",
      "numinlets" : 1,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 10, 100, 400, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.numbox @min 0 @max 11 @inc 1",
      "numinlets" : 2,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 420, 100, 80, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.text @text \"SEQUENCER 1\"",
      "numinlets" : 1,
      "numoutlets" : 1,
      "outlettype" : [ "" ],
      "patching_rect" : [ 10, 140, 200, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.text @text \"SEQUENCER 2\"",
      "numinlets" : 1,
      "numoutlets" : 1,
      "outlettype" : [ "" ],
      "patching_rect" : [ 300, 140, 200, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.menu @items Forward Reverse Converge Random",
      "numinlets" : 1,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 10, 170, 280, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.numbox @min 0 @max 100 @inc 1",
      "numinlets" : 2,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 10, 200, 80, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.toggle",
      "numinlets" : 1,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 100, 200, 40, 40 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "matrixctrl 16 1 @mode 2 @rows 16 @cols 1",
      "numinlets" : 1,
      "numoutlets" : 2,
      "outlettype" : [ "list", "list" ],
      "patching_rect" : [ 10, 260, 100, 320 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "matrixctrl 16 1 @mode 2 @rows 16 @cols 1",
      "numinlets" : 1,
      "numoutlets" : 2,
      "outlettype" : [ "list", "list" ],
      "patching_rect" : [ 130, 260, 100, 320 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "matrixctrl 16 3 @mode 2 @rows 16 @cols 3",
      "numinlets" : 1,
      "numoutlets" : 2,
      "outlettype" : [ "list", "list" ],
      "patching_rect" : [ 10, 600, 120, 320 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.numbox @min 0 @max 127 @inc 1",
      "numinlets" : 2,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 150, 600, 80, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.menu @items Sine Saw Square Triangle",
      "numinlets" : 1,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 10, 950, 250, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.numbox @min 0 @max 100 @inc 1",
      "numinlets" : 2,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 270, 950, 80, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.numbox @min 0 @max 100 @inc 1",
      "numinlets" : 2,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 360, 950, 80, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.numbox @min 0 @max 100 @inc 1",
      "numinlets" : 2,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 450, 950, 80, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.menu @items LP BP HP",
      "numinlets" : 1,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 10, 1000, 180, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.numbox @min 0 @max 100 @inc 1",
      "numinlets" : 2,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 200, 1000, 80, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.numbox @min 0 @max 100 @inc 1",
      "numinlets" : 2,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 290, 1000, 80, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.numbox @min 0 @max 100 @inc 1",
      "numinlets" : 2,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 380, 1000, 80, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.numbox @min 0 @max 100 @inc 1",
      "numinlets" : 2,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 470, 1000, 80, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.menu @items Sine Triangle Square Saw",
      "numinlets" : 1,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 10, 1050, 280, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.numbox @min 0 @max 100 @inc 1",
      "numinlets" : 2,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 300, 1050, 80, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.numbox @min 0 @max 100 @inc 1",
      "numinlets" : 2,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 390, 1050, 80, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.numbox @min 0 @max 100 @inc 1",
      "numinlets" : 2,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 480, 1050, 80, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.numbox @min 0 @max 100 @inc 1",
      "numinlets" : 2,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 570, 1050, 80, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.numbox @min 0 @max 100 @inc 1",
      "numinlets" : 2,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 10, 1100, 80, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.numbox @min 0 @max 100 @inc 1",
      "numinlets" : 2,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 100, 1100, 80, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.numbox @min 0 @max 100 @inc 1",
      "numinlets" : 2,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 190, 1100, 80, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.numbox @min 0 @max 100 @inc 1",
      "numinlets" : 2,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 280, 1100, 80, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.text @text \"OSCILLATOR 1\"",
      "numinlets" : 1,
      "numoutlets" : 1,
      "outlettype" : [ "" ],
      "patching_rect" : [ 10, 920, 200, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.text @text \"FILTER\"",
      "numinlets" : 1,
      "numoutlets" : 1,
      "outlettype" : [ "" ],
      "patching_rect" : [ 10, 970, 200, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.text @text \"OSC2 / FM\"",
      "numinlets" : 1,
      "numoutlets" : 1,
      "patching_rect" : [ 10, 1020, 200, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.text @text \"MODULATION\"",
      "numinlets" : 1,
      "numoutlets" : 1,
      "patching_rect" : [ 10, 1070, 200, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "live.numbox @min 0 @max 16 @inc 1",
      "numinlets" : 2,
      "numoutlets" : 2,
      "outlettype" : [ "", "" ],
      "patching_rect" : [ 10, 1150, 80, 22 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "button",
      "numinlets" : 1,
      "numoutlets" : 1,
      "outlettype" : [ "bang" ],
      "patching_rect" : [ 100, 1150, 24, 24 ]
    }
  }, {
    "box" : {
      "maxclass" : "newobj",
      "text" : "button",
      "numinlets" : 1,
      "numoutlets" : 1,
      "outlettype" : [ "bang" ],
      "patching_rect" : [ 140, 1150, 24, 24 ]
    }
  } ],
  "lines" : [ ],
  "version" : 1,
  "formatversion" : 1,
  "metainfo" : {
    "default_fontsize" : 12.0,
    "default_fontname" : "Arial"
  },
  "patching_rect" : [ 0, 0, 640, 1200 ]
}
