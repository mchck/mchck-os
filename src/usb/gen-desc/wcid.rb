class WcidDesc < GlobalDesc
  @@wcid_id = 0

  child_block :wcid

  def initialize
    super

    control_func :wcid_handle_control

    @wcid_id = @@wcid_id
    @@wcid_id += 1

    self.device.add_string_raw("USB_DESC_WCID_OS", "WCID_REQ_ID")
  end

  def gen_init(nextid)
    <<_end_
#{super(nextid)}
	.compat_id = &wcid_data_#@wcid_id.header,
_end_
  end

  def gen_vars(nextid)
    wfs = parent.get_config.first.get_function.map do |f|
      wcid = f.get_wcid
      next if wcid.nil?
      [f.get_interface.first.ifacenum, wcid]
    end.compact

    s = <<_end_
static const struct {
	struct wcid_compat_id_header header;
	struct wcid_compat_id_function funcs[#{wfs.count}];
} wcid_data_#@wcid_id = {
	.header = {
		.dwLength = sizeof(wcid_data_#@wcid_id),
		.bcdVersion = { .raw = 0x0100 },
		.wIndex = WCID_DESC_COMPAT_OS,
		.bCount = #{wfs.count},
	},
	.funcs = {
_end_
    wfs.each do |fid, wcid|
      compid = wcid
      subcompid = ""

      if Array === compid
        compid, subcompid = compid
      end

      s += <<_end_
		{
			.bFirstInterfaceNumber = #{fid},
			._rsvd = 1,
			.compatibleID = "#{compid}",
			.subCompatibleID = "#{subcompid}",
		},
_end_
    end
    s += <<_end_
	}
};

const struct wcid_function #@var_name = {
#{gen_init(nextid)}
};

_end_

    s
  end
end
