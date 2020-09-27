var hot = [{
    "pos": 0,
    "r": 0,
    "g": 0,
    "b": 0
  },
  {
    "pos": 84,
    "r": 255,
    "g": 0,
    "b": 0
  },
  {
    "pos": 173,
    "r": 255,
    "g": 255,
    "b": 0
  },
  {
    "pos": 255,
    "r": 255,
    "g": 255,
    "b": 255
  }
];

var aqua = [{
    "pos": 0,
    "r": 0,
    "g": 0,
    "b": 0
  },
  {
    "pos": 84,
    "r": 0,
    "g": 0,
    "b": 255
  },
  {
    "pos": 173,
    "r": 0,
    "g": 255,
    "b": 255
  },
  {
    "pos": 255,
    "r": 255,
    "g": 255,
    "b": 255
  }
];


// Fake HTTP request
var defaultData = {
  timestamp: "2020-09-07T20:29:19.882Z",
  size: 50,
  selected: "fire",
  effects: {
    fire: {
      name: "fire",
      data: {
        gradient: {
          type: "gradient",
          value: aqua,
          /* callback: function() {}, */
          mapto: "PALETTE"
        },
        brightness: {
          type: "range",
          class: "enabled",
          value: 64,
          min: 0,
          max: 255,
          step: 1,
          /* callback: caller, */
        },
        speed: {
          type: "range",
          class: "enabled",
          value: 60,
          min: 10,
          max: 60,
          step: 1,
          /* callback: caller, */
          mapto: "FRAMES_PER_SECOND"
        },
        intensity: {
          type: "range",
          class: "enabled",
          value: 80,
          min: 50,
          max: 200,
          step: 1,
          /* callback: caller, */
          mapto: "SPARKING"
        },
        size: {
          type: "range",
          class: "enabled",
          value: 50,
          min: 20,
          max: 100,
          step: 1,
          /* callback: caller, */
          mapto: "COOLING",
          reverse: true
        },
      },

    },
    scanner: {
      name: "scanner",
      data: {
        color: {
          type: "color",
          class: "colorpick",
          /* callback: caller, */
          value: [0, 0, 255],
        },
        speed: {
          type: "range",
          class: "enabled",
          value: 2,
          min: 1,
          max: 10,
          step: 1,
          reverse: true,
          /* callback: caller, */
        },
        spread: {
          type: "range",
          class: "enabled",
          value: 12,
          min: 1,
          max: 20,
          step: 1,
          /* callback: caller, */
        },
      }
    },


  }
}







function PillarOfLight(params) {

  this.d = params;
  this.name = "Pillar of light"
  this.data = params.effects || {}
  this.effect = params.selected || "fire"
  this.container = params.container || $('.simulation')[0]; // Container DIV in the page
  this.size = params.size >= 85 ? params.size : 85;
  this.ajv = false;

  return this;
}

PillarOfLight.prototype.setValidator = function(ajv) {
  this.ajv = ajv;
}


PillarOfLight.prototype.show = function() {
  if (this.d) {
    let effect = this.data[this.effect]
    this.InsertEffectsSelector()
    this.InsertControls(effect)
    this.StartSimulation()
  }
}


PillarOfLight.prototype.updateAll = function() {

  let datetime = new Date()
  this.d.timestamp = datetime.toISOString()

  this.updateSimulation()
  this.sendDataAsJSON()

}


PillarOfLight.prototype.updateSimulation = function() {
  let data = this.data[this.effect].data
  this.driver.update(data, this.opts)
}


PillarOfLight.prototype.sendDataAsJSON = function() {
  let valid = false;
  /* Validate before sending */
  if (this.ajv) {
    valid = this.ajv.validate('pillarSchema', this.d);
  } else {
    valid = true; // If not choosing to validate, then assume valid
  }

  /*   If invalid, send error */
  if (!valid) {
    console.warn(this.ajv.errorsText())
    return
  }

  let dataToSend = this.d.effects[this.d.selected];
  connection.send(JSON.stringify(dataToSend));

}


PillarOfLight.prototype.InsertEffectsSelector = function() {

  // Insert Material Design Icon & Label
  $("<i>").appendTo(".effectSelector").addClass("material-icons").text("movie_filter")
  $('<span>').appendTo(".effectSelector").text("effect")

  var select = $('<select>').appendTo(".effectSelector")
  select.addClass("select-css")
  select.on('change', ((e) => {
    /* // Cancel current animation on change
    animation = cancelAnimationFrame(animation) */
    let selectedEffect = e.target.options[e.target.selectedIndex].value
    let effect = this.data[selectedEffect]
    this.effect = selectedEffect
    this.d.selected = selectedEffect
    this.RemoveControls()
    this.InsertControls(effect)
    this.setEffect()

  }).bind(this))

  for (const effectName in this.data) {

    var effect = this.data[effectName]
    var option = $('<option>').appendTo(select)
    option.attr("value", effectName)
    option.text(effectName)

    if (effectName == this.effect) {
      option.attr("selected", "selected")
    }

  }
}


PillarOfLight.prototype.InsertRangeSlider = function(ctrlTarget) {

  let iconMap = {
    brightness: "iso",
    speed: "speed",
    intensity: "local_fire_department",
    size: "straighten",
    spread: "graphic_eq"
  }

  // Insert Material Design Icon
  $("<i>").appendTo(ctrlTarget.container).addClass("material-icons").text(iconMap[ctrlTarget.name])
  $("<span>").appendTo(ctrlTarget.container).text(ctrlTarget.name)

  ctrlTarget.model.id = ctrlTarget.name + "-slider"
  //ctrlTarget.label.text(ctrlTarget.name);

  //ctrlTarget.model["data-mapto"] = ctrlTarget.model.mapto
  var input = $("<input>").appendTo(ctrlTarget.container).attr(ctrlTarget.model) //.on("input", ctl.callback);


  /* Reverse control direction */
  if (ctrlTarget.model.reverse) {
    $(input).css("direction", "rtl")
  }

  /* Critical Data Binding */
  this.BindData(ctrlTarget.model, input[0], ctrlTarget.name)

  /* Slider Specific Code */
  this.setSliderColor(input[0])
  input[0].addEventListener('input', (event) => {
    let slider = event.target
    this.setSliderColor(slider)
  })
}


PillarOfLight.prototype.InsertColorPicker = function(ctrlTarget) {

  // Insert Material Design Iconn & Label
  $("<i>").appendTo(ctrlTarget.container).addClass("material-icons").text("palette")
  $("<span>").appendTo(ctrlTarget.container).text(ctrlTarget.name)

  let val = ctrlTarget.model.value
  let hexStr = "#" + tinycolor({
    "r": val[0],
    "g": val[1],
    "b": val[2]
  }).toHex()
  ctrlTarget.model.value = hexStr
  //ctrlTarget.label.text("color");



  var input = $("<input>").appendTo(ctrlTarget.container).attr(ctrlTarget.model)

  input.ready(function() {
    let colorPicker = document.querySelector("[type=color]")
    let newColor = colorPicker.value
    colorPicker.value = "#f0f0f1" // hack, otherwise next line doesn't work
    colorPicker.value = newColor
  })


  ctrlTarget.model.value = val

  input[0].addEventListener('input', (event) => {
    let curColor = event.target.value
    let rgb = tinycolor(curColor).toRgb()
    let rgbArray = [rgb.r, rgb.g, rgb.b]
    ctrlTarget.model.value = rgbArray
    //console.log(event.target.value)
  })

  input[0].addEventListener('input', this.updateAll.bind(this))
}


PillarOfLight.prototype.InsertGradientPicker = function(ctrlTarget) {

  // Insert Material Design Icon & Label
  $("<i>").appendTo(ctrlTarget.container).addClass("material-icons").text("gradient")
  $("<span>").appendTo(ctrlTarget.container).text(ctrlTarget.name)

  // Insert Gradient
  var gradient = $("<div>").appendTo(ctrlTarget.container).addClass("gradient")

  // load gradient control 
  const gp = new Grapick({
    el: '.gradient'
  });

  // Handlers are color stops
  if (typeof(ctrlTarget.model.value) != "undefined") {
    ctrlTarget.model.value.forEach(color => {
      var rgbString = tinycolor(color).toRgbString()
      var position = Number(color.pos) * 100 / 255
      gp.addHandler(position, rgbString)
    })
  } else { // fallback settings
    // Handlers are color stops
    gp.addHandler(0, 'black')
    gp.addHandler(33, 'blue')
    gp.addHandler(66, 'aqua')
    gp.addHandler(100, 'white')
  }


  // Do stuff on change of the gradient
  gp.on('change', complete => {

    ctrlTarget.model.value = this.getGradient().colors
    this.updateAll()

  });
}


PillarOfLight.prototype.InsertControls = function(effect) {

  /* Create Table element to contain controls */

  for (let [name, ctl] of Object.entries(effect.data)) {

    if (typeof(ctl) != 'undefined') {

      let ctrlTarget = {
        "name": name,
        "container": $("<div>").appendTo(".controls"),
        "model": ctl
      }

      if (ctl.type === "range") {
        this.InsertRangeSlider(ctrlTarget)
      } else if (ctl.type === "color") {
        this.InsertColorPicker(ctrlTarget)
      } else if (ctl.type === "gradient") {
        this.InsertGradientPicker(ctrlTarget)
      } else {
        console.warn(ctl.type + " control type not supported")
      }

    } else {
      console.warn("Control Undefined!")
    }
  }


}


PillarOfLight.prototype.RemoveControls = function() {
  $('.controls').empty()
}


PillarOfLight.prototype.StartSimulation = function() {

  //LEDstrip.prototype.send = sendMonkeyPatch
  LEDstrip.prototype.reversePixels = reversePixels
  // New instance of LEDstrip object

  this.strip = LEDstrip(this.container, this.size)
  this.strip.send = sendMonkeyPatch.bind(this.strip)
  this.strip.brightness = 255

  // Reverse order of pixels in the strip
  this.strip.reversePixels()

  /* Which effect will get loaded? */
  this.setEffect()
}

/* TODO: Obviate need for this function
Probably can use DOM attribute such as data-mapto */
PillarOfLight.prototype.setOpts = function() {

  this.opts = {}
  var effect = this.data[this.effect]

  for (let [name, obj] of Object.entries(effect.data)) {
    if (obj.mapto) {
      this.opts[obj.mapto] = name
    } else {
      //default mapping is to use object name for simulation paramter
      this.opts[name] = name
    }
  }
}


PillarOfLight.prototype.setEffect = function() {

  // Cancel current animation on change
  if (typeof animation != 'undefined') {
    animation = cancelAnimationFrame(animation)
  }

  /* Select effect to be displayed */
  switch (this.effect) {
    case "fire":
      this.driver = new Fire(this.strip)
      break
    case "scanner":
      this.driver = new Larson(this.strip)
      break

      /* other cases: flares, chasers, bouncingballs, lightning, twinklesparkle, torture, wave, cycle, eyeblink
       */

  }

  this.driver.update = updateGeneral
  this.setOpts()
  this.driver.init()
  this.driver.animate()
  this.driver.update(this.data[this.effect].data, this.opts)
}


PillarOfLight.prototype.saveData = function() {

}


PillarOfLight.prototype.BindData = function(ctl, input, name) {
  ctl.name = name
  new Binding({
    object: ctl,
    property: "value",
    element: input,
    attribute: "value",
    event: "input",
    callContext: this,
  })

}


PillarOfLight.prototype.setSliderColor = function(slider) {
  let direction = $(slider).css("direction")
  let percentage = 100 * (slider.value - slider.min) / (slider.max - slider.min)

  if (direction == "rtl") {
    percentage = 100 - percentage
  }

  let base = tinycolor($(slider).css("background-color"))
  let rightColor = base.toRgbString();
  let leftColor = base.lighten(20).toRgbString();

  let newBackColor = "rgba(230, 230, 230, 1)"

  let background = "linear-gradient(to right, " + leftColor + " 0%, " + rightColor + " " + percentage + "%, " + newBackColor + " " + (percentage + 0.1) + "%)";

  $(slider).css("background-image", background)
}


PillarOfLight.prototype.getGradient = function() {

  const typeName = name => `${name}-gradient(`;

  /* let value = document.body.style.background; */
  let value = $('.grp-preview')[0].style.backgroundImage
  let start = value.indexOf('(') + 1;
  let end = value.lastIndexOf(')');
  let gradients = value.substring(start, end);
  const values = gradients.split(/,(?![^(]*\)) /);

  var result = {
    type: '',
    direction: '',
    colors: [],
  };

  if (values.length > 2) {
    result.direction = values.shift();
  }

  let typeFound;
  const types = ['repeating-linear', 'repeating-radial', 'linear', 'radial'];
  types.forEach(name => {
    if (value.indexOf(typeName(name)) > -1 && !typeFound) {
      typeFound = 1;
      result.type = name;
    }
  })

  values.forEach(value => {
    const hdlValues = value.split(' ');
    const position = Math.round(2.55 * parseFloat(hdlValues.pop()));
    const color = hdlValues.join('');
    var c = tinycolor(color);
    result.colors.push({
      pos: position,
      r: c._r,
      g: c._g,
      b: c._b
    });
  });

  // make sure first and last values are 0 and 255, accordingly
  result.colors[0].pos = 0;
  result.colors[result.colors.length - 1].pos = 255;

  return result;
}







function Binding(b) {
  var _this = this
  this.element = b.element
  this.value = b.object[b.property]
  this.attribute = b.attribute
  this.valueGetter = function() {
    return _Number(_this.value);
  }
  this.valueSetter = function(val) {
    _this.value = _Number(val)
    _this.element[_this.attribute] = _Number(val)
  }

  if (b.event) {
    this.element.addEventListener(b.event, function(event) {
      _this.value = _this.element[_this.attribute]
    })
  }

  if (b.object.callback) {
    /* this.element.addEventListener(b.event, b.object.callback.bind(b.callContext)) */
  } else {
    this.element.addEventListener(b.event, b.callContext.updateAll.bind(b.callContext))
  }

  Object.defineProperty(b.object, b.property, {
    get: this.valueGetter,
    set: this.valueSetter
  });
  b.object[b.property] = this.value;

  this.element[this.attribute] = this.value

  function _Number(val) {
    // Cast to number if possible, else return original value
    var num = Number(val)
    if (isNaN(num)) return val
    return num
  }
}









function reversePixels() {
  let container = this.elem
  $.fn.reverseChildren = function() {
    return this.each(function() {
      var $this = $(this);
      $this.children().each(function() {
        $this.prepend(this)
      })
    })
  }
  $(container).reverseChildren()
}













function updateGeneral(effect, opts) {
  animation = cancelAnimationFrame(animation)
  Object.keys(opts).forEach(key => {
    var param = opts[key]
    this[key] = effect[param].value
  });

  // Set brightness if available
  if (effect.brightness) {
    this.ledstrip.brightness = Number(effect.brightness.value)
  } else {
    this.ledstrip.brightness = 255
  }

  //console.log(this)
  this.animate()
}





function sendMonkeyPatch() {

  var pushrgb = this.pushrgb.bind(this); // capture scope
  var gamma = 1 / 8
  var brightness = Math.pow(Number(this.brightness), gamma) / Math.pow(255, gamma) // 255^4
  //console.log("brightness: " + brightness + " color: " + this.color)



  this.buffer.forEach(function(val) {
    //console.log(val)
    for (var i = 0; i < 3; i++) {
      val[i] = Math.round(val[i] * brightness)
    }
    pushrgb(val);
  });

  this.latch();

  return this;
}





async function fetchValidJSON(ajv, jsonUrl, schemaUrl) {
  // Fetch both urls in parallel, but wait until both have resolved
  let [res1, res2] = await Promise.all([fetch(jsonUrl), fetch(schemaUrl)])

  if (res1.status != 200) {
    // Couldn't find JSON
    throw new Error("Invalid JSON URL")
  } else if (res2.status != 200) {
    // Couldn't Find Schema
    throw new Error("Invalid Schema URL")
  } else {
    // We good
    let json = await res1.json()
    let schema = await res2.json()

    // debug - force validation error
    //json.timestamp = "Erroneous Data"

    // do some validation, throw error if not valid
    ajv.addSchema(schema, 'pillarSchema');
    let valid = ajv.validate('pillarSchema', json);
    if (!valid) {
      throw new Error("Failed Validation: " + ajv.errorsText())
    }

    return json
  }

}


/*
	By Osvaldas Valutis, www.osvaldas.info
	Available for use under the MIT License
*/
function styleFileInput() {

  var inputs = document.querySelectorAll('.inputfile');

  Array.prototype.forEach.call(inputs, function(input) {
    let label = input.labels[0];
    let labelVal = label.innerHTML;

    input.addEventListener('change', function(e) {
      var fileName = '';
      if (this.files && this.files.length > 1)
        fileName = (this.getAttribute('data-multiple-caption') || '').replace('{count}', this.files.length);
      else
        fileName = e.target.value.split('\\').pop();

      if (fileName)
        label.querySelector('span').innerHTML = fileName;
      else
        label.innerHTML = labelVal;
    });

    // Firefox bug fix
    input.addEventListener('focus', function() {
      input.classList.add('has-focus');
    });
    input.addEventListener('blur', function() {
      input.classList.remove('has-focus');
    });
  });

}

/* function parseISOString(s) {
  var b = s.split(/\D+/);
  return new Date(Date.UTC(b[0], --b[1], b[2], b[3], b[4], b[5], b[6]));
} */

