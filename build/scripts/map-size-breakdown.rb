class MapFile
  class Section
    attr_accessor :name, :size, :members

    def initialize(name, size)
      @name, @size = name, Integer(size)
      @members = []
    end

    def <<(m)
      m = Section.new(m, 0) unless Section === m
      @members << m
    end

    def each(&block)
      @members.each(&block)
    end

    def sorted_members
      @members.sort_by{|e| -e.size}
    end

    def drop!(re)
      @members.delete_if{|e| re === e.name}
    end

    def to_s
      "#@name #@size\n" +
        self.sorted_members.map{|m| m.to_s.split("\n").map{|l| "  "+l}.join("\n")}.join("\n")
    end
  end

  attr_accessor :sections

  def initialize(file)
    @file = file
    @sections = Section.new('root', 0)
    @cursect = nil
  end

  def parse!
    state = :skip
    File.readlines(@file).each do |l|
      if state == :skip
        state = :wait_section if l.match /^Linker script and memory map/
        next
      end

      case l
      when /^[.]/
        state = :section
        @sectline = ''
      when /^ [^ ]/
        state = :membersect
        @memberline = ''
      when /^$/
        state = :wait_section
      end

      case state
      when :section
        @sectline += l
        sect, _, size = @sectline.strip.split

        if size
          @cursect = Section.new(sect, size)
          @sections << @cursect
          state = :wait_member
        end
      when :membersect
        @memberline += l
        if m = @memberline.match(/^\s+(?<membersect>[^\s]+)\s+0x[0-9a-f]+\s+(?<size>0x[0-9a-f]+)\s*(?<obj>.*)/)
          subname = m['membersect']
          if !m['obj'].empty?
            subname += " (#{m['obj']})"
          end
          @curmembersect = Section.new(subname, m['size'])
          @cursect << @curmembersect
          state = :membersym
          @memberline = ''
        end
      when :membersym
        if m = l.match(/^\s+0x[0-9a-f]+\s+(?<sym>.*?)\s*$/)
          if m['sym'].match(/^[.] = /)
            # offset correction, ignore
          else
            @curmembersect << m['sym']
          end
        end
      end
    end
  end
end

if $0 == __FILE__
  m = MapFile.new(ARGV[0])
  m.parse!
  s = m.sections
  s.drop!(/^[.]debug/)
  s.drop!(/^[.]comment/)
  s.each do |sect|
    sect.each do |subs|
      if subs.members.size == 1 && subs.members.first.size == 0
        subs.name = subs.members.first.name
        subs.members = []
      end
    end
  end

  printer = ->(sect, prefix) {
    total = sect.size
    cumulative = 0
    sect.sorted_members.select{|m| m.size > 0}.each do |m|
      cumulative += m.size
      puts "%s %1.3f %1.3f %s %s" % [prefix, m.size.to_f/total, cumulative.to_f/total, m.name, m.size]
      printer.(m, prefix+"  ")
    end
  }
  printer.(s, "")
end
