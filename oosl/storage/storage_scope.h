#pragma once

#ifndef OOSL_STORAGE_SCOPE_H
#define OOSL_STORAGE_SCOPE_H

namespace oosl{
	namespace storage{
		class scope{
		public:
			scope();

			explicit scope(scope &parent);

			~scope();

			void on_thread_entry();

			void on_thread_exit();

			scope *parent() const;

		private:
			scope *parent_;
		};
	}
}

#endif /* !OOSL_STORAGE_SCOPE_H */
