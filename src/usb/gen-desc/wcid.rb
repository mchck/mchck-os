class WcidDesc < FunctionDesc
  TypeName = "struct wcid_function"

  child_block :wcid

  def initialize
    super

    init_func :wcid_init
    control_func :wcid_handle_control

    # XXX string desc
  end
end
