/*
 * \brief  VM session component for 'base-hw'
 * \author Stefan Kalkowski
 * \date   2012-10-08
 */

/*
 * Copyright (C) 2012-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include <vm_session_component.h>
#include <platform.h>
#include <core_env.h>

using namespace Genode;


addr_t Vm_session_component::_alloc_ds(size_t &ram_quota)
{
	addr_t addr;
	if (_ds_size() > ram_quota ||
		platform()->ram_alloc()->alloc_aligned(_ds_size(), (void**)&addr,
		                                       get_page_size_log2()).is_error())
		throw Root::Quota_exceeded();
	ram_quota -= _ds_size();
	return addr;
}


void Vm_session_component::run(void)
{
	if (Kernel_object<Kernel::Vm>::_cap.valid())
		Kernel::run_vm(kernel_object());
}


void Vm_session_component::pause(void)
{
	if (Kernel_object<Kernel::Vm>::_cap.valid())
		Kernel::pause_vm(kernel_object());
}


Vm_session_component::~Vm_session_component()
{
	/* dissolve VM dataspace from service entry point */
	_ds_ep->dissolve(&_ds);

	/* free region in allocator */
	core_env()->rm_session()->detach(_ds.core_local_addr());
	platform()->ram_alloc()->free((void*)_ds.phys_addr());
}
