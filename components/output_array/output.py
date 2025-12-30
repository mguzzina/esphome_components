from esphome import pins
import esphome.codegen as cg
from esphome.components import output, sigma_delta_output
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_OUTPUT_ID, CONF_PINS

output_array_ns = cg.esphome_ns.namespace("output_array")
GPIOWrapper = output_array_ns.class_("GPIOWrapper", cg.GPIOPin)
OutputArray = output_array_ns.class_(
    "OutputArray", output.FloatOutput, cg.Component, GPIOWrapper
)

DEPENDENCIES = []

CONFIG_SCHEMA = cv.All(
    output.FLOAT_OUTPUT_SCHEMA.extend(
        {
            cv.Required(CONF_ID): cv.declare_id(OutputArray),
            cv.Required(CONF_OUTPUT_ID): cv.use_id(
                sigma_delta_output.output.SigmaDeltaOutput
            ),
            cv.Required(CONF_PINS): cv.ensure_list(pins.gpio_output_pin_schema),
        }
    )
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await output.register_output(var, config)

    assert len(config[CONF_PINS]) > 0

    pin_list = []
    for pin_id in config[CONF_PINS]:
        pin_list.append(await cg.gpio_pin_expression(pin_id))
    cg.add(var.set_outputs(await cg.get_variable(config[CONF_OUTPUT_ID]), pin_list))

    await cg.register_component(var, config)
