class CDCDesc < FunctionDesc
  TypeName = "struct cdc_function_desc"

  child_block :cdc

  field :data_ready, :optional => true
  field :data_sent, :optional => true

  def initialize(name)
    @name = name
    super()

    init_func :cdc_init

    bFunctionClass :USB_DEV_CLASS_CDC
    bFunctionSubClass :USB_DEV_CLASS_CDC
    bFunctionProtocol 0

    @ctrl_iface = interface(:ctrl_iface) {
      bInterfaceClass :USB_DEV_CLASS_CDC
      bInterfaceSubClass :USB_DEV_SUBCLASS_CDC_ACM
      bInterfaceProtocol 0
      ep(:ctrl_ep) {
        direction :in
        type :intr
        wMaxPacketSize :CDC_NOTICE_SIZE
        bInterval 0xff
      }
    }
    @data_iface = interface(:data_iface) {
      bInterfaceClass :USB_DEV_CLASS_CDC_DCD
      bInterfaceSubClass 0
      bInterfaceProtocol 0
      ep(:tx_ep) {
        direction :in
        type :bulk
        wMaxPacketSize :CDC_TX_SIZE
        bInterval 0xff
      }
      ep(:rx_ep) {
        direction :out
        type :bulk
        wMaxPacketSize :CDC_RX_SIZE
        bInterval 0xff
      }
    }
  end

  def gen_defs
    s = super()
    s += <<_end_
extern struct cdc_ctx #{@name.to_loc_s};
_end_
    s += "cdc_data_ready_cb_t #{@data_ready.to_loc_s};\n" if !@data_ready.nil?
    s += "cdc_data_sent_cb_t #{@data_sent.to_loc_s};\n" if !@data_sent.nil?
    s
  end

  def gen_vars
    s = <<_end_
struct cdc_ctx #{@name.to_loc_s};
_end_
    s += super
  end

  def gen_func_defs
    <<_end_
	struct cdc_function cdc_func;
_end_
  end

  def gen_func_init
    s = <<_end_
	.cdc_func = {
		.usb_func = #{super}
		.ctx = &#{@name.to_loc_s},
_end_

    s += "\t\t.data_ready_cb = #{@data_ready.to_loc_s},\n" if !@data_ready.nil?
    s += "\t\t.data_sent_cb = #{@data_sent.to_loc_s},\n" if !@data_sent.nil?
    s +=
<<_end_
	},
_end_
    s
  end

  def get_function_var
    super("cdc_func.usb_func")
  end

  def gen_desc_init
    <<_end_
.#@var_name = {
	.iad = #{self.gen_iad_desc_init},
	#{@interface.map{|i| i.gen_desc_init}.join}
	.cdc_header = {
		.bLength = sizeof(struct cdc_desc_function_header_t),
		.bDescriptorType = {
			.id = USB_DESC_IFACE,
			.type_type = USB_DESC_TYPE_CLASS
		},
		.bDescriptorSubtype = USB_CDC_SUBTYPE_HEADER,
		.bcdCDC = { .maj = 1, .min = 1 }
	},
	.cdc_acm = {
		.bLength = sizeof(struct cdc_desc_acm_header_t),
		.bDescriptorType = {
			.id = USB_DESC_IFACE,
			.type_type = USB_DESC_TYPE_CLASS
		},
		.bDescriptorSubtype = USB_CDC_SUBTYPE_ACM,
		.bmCapabilities = 6
	},
	.cdc_call = {
		.bLength = sizeof(struct cdc_desc_call_header_t),
		.bDescriptorType = {
			.id = USB_DESC_IFACE,
			.type_type = USB_DESC_TYPE_CLASS
		},
		.bDescriptorSubtype = USB_CDC_SUBTYPE_CALL,
		.bmCapabilities = 0,
		.bDataInterface = #{@data_iface.ifacenum}
	},
	.cdc_union = {
		.bLength = sizeof(struct cdc_desc_function_union_t),
		.bDescriptorType = {
			.id = USB_DESC_IFACE,
			.type_type = USB_DESC_TYPE_CLASS
		},
		.bDescriptorSubtype = USB_CDC_SUBTYPE_UNION,
		.bControlInterface = #{@ctrl_iface.ifacenum},
		.bSubordinateInterface0 = #{@data_iface.ifacenum}
	},
},
_end_
  end
end
