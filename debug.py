
import gdb

bkpaddr = '*0x7c77'

class ShowCpuInfo(gdb.Breakpoint):
    def stop(self):
        # load symbol
        gdb.execute('set confirm off')
        gdb.execute('add-symbol-file kernel.elf 0x21000')

        # get variables
        cpu_info = int(gdb.parse_and_eval('&cpu_info'))
        # cpu_info = gdbvalue.cast(gdb.lookup_type('long').pointer()).dereference()

        num_of_cpus = int(gdb.parse_and_eval('num_of_cpus'))
        # num_of_cpus = gdbvalue.cast(gdb.lookup_type('long').pointer()).dereference()

        # dump cpu_info structure
        for i in range(num_of_cpus):
            ptr = cpu_info + i * 16

            print('')
            value = gdb.Value(ptr).cast(gdb.lookup_type('unsigned long').pointer()).dereference()
            print('cpu_info[%d].isbsp=0x%x' % (i, int(value)))

            value = gdb.Value(ptr + 4).cast(gdb.lookup_type('unsigned long').pointer()).dereference()
            print('cpu_info[%d].lapic_id=0x%x' % (i, int(value)))

            value = gdb.Value(ptr + 8).cast(gdb.lookup_type('unsigned long').pointer()).dereference()
            print('cpu_info[%d].lapic_base.low=0x%x' % (i, int(value)))

            value = gdb.Value(ptr + 12).cast(gdb.lookup_type('unsigned long').pointer()).dereference()
            print('cpu_info[%d].lapic_base.high=0x%x' % (i, int(value)))
            print('')

        return True

gdb.execute('target remote localhost:1234')
ShowCpuInfo(bkpaddr)
gdb.execute('continue')
