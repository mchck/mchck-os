class DFUDesc < FunctionDesc
  FunctionVarName = "dfu_function"

  child_block :dfu

  field :segment, :optional => true, :list => true

  def initialize
    super

    wcid "WINUSB"
  end

  def post_eval
    super

    segments = @segment
    seg = segments.first || 0

    seg2sym = ->(seg) do
      case seg
      when Array, Hash
        seg = seg.to_a
      else
        seg = [seg]
      end
      sym, name = seg
      name ||= sym.to_s

      [sym, name]
    end

    interface(:iface) {
      bInterfaceClass :USB_DEV_CLASS_APP
      bInterfaceSubClass :USB_DEV_SUBCLASS_APP_DFU
      bInterfaceProtocol :USB_DEV_PROTO_DFU_DFU
      sym, name = seg2sym.(seg)
      iInterface name if segments.any?

      segments[1..-1].each_with_index do |seg, idx|
        alternate("iface#{idx}") {
          bInterfaceClass :USB_DEV_CLASS_APP
          bInterfaceSubClass :USB_DEV_SUBCLASS_APP_DFU
          bInterfaceProtocol :USB_DEV_PROTO_DFU_DFU
          sym, name = seg2sym.(seg)
          iInterface name
        }
      end
    }
  end

  def get_desc_struct
    super +
      <<_end_
	struct dfu_desc_functional dfu;
_end_
  end

  def gen_desc_init
    super +
      <<_end_
	.dfu = {
		.bLength = sizeof(struct dfu_desc_functional),
		.bDescriptorType = {
			.id = 0x1,
			.type_type = USB_DESC_TYPE_CLASS
		},
		.will_detach = 1,
		.manifestation_tolerant = 1,
		.can_upload = 0,
		.can_download = 1,
		.wDetachTimeOut = 0,
		.wTransferSize = USB_DFU_TRANSFER_SIZE,
		.bcdDFUVersion = { .maj = 1, .min = 1 }
	},
_end_
  end
end

class DFUAppDesc < FunctionDesc
  TypeName = "struct dfu_function_desc"
  FunctionVarName = "dfu_app_function"

  child_block :dfu_app

  def initialize
    super

    interface(:iface) {
      bInterfaceClass :USB_DEV_CLASS_APP
      bInterfaceSubClass :USB_DEV_SUBCLASS_APP_DFU
      bInterfaceProtocol :USB_DEV_PROTO_DFU_APP
    }
  end

  def gen_desc_init
    <<_end_
.#@var_name = {
	#{@interface.first.gen_desc_init}
	.dfu = {
		.bLength = sizeof(struct dfu_desc_functional),
		.bDescriptorType = {
			.id = 0x1,
			.type_type = USB_DESC_TYPE_CLASS
		},
		.will_detach = 1,
		.manifestation_tolerant = 0,
		.can_upload = 0,
		.can_download = 1,
		.wDetachTimeOut = 0,
		.wTransferSize = USB_DFU_TRANSFER_SIZE,
		.bcdDFUVersion = { .maj = 1, .min = 1 }
	}
},
_end_
  end
end
