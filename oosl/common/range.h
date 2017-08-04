#pragma once

#ifndef OOSL_RANGE_H
#define OOSL_RANGE_H

namespace oosl{
	namespace common{
		template <class value_type>
		class range{
		public:
			typedef value_type value_type;

			range(value_type left, value_type right)
				: left_(left), right_(right){}

			bool operator <(const range &rhs) const{
				if (left_ != rhs.left_)
					return (left_ < rhs.left_);
				return (right_ < rhs.right_);
			}

			bool operator <=(const range &rhs) const{
				return (*this < rhs || *this == rhs);
			}

			bool operator ==(const range &rhs) const{
				return (left_ == rhs.left_ && right_ == rhs.right_);
			}

			bool operator !=(const range &rhs) const{
				return !(*this == rhs);
			}

			bool operator >=(const range &rhs) const{
				return !(*this < rhs);
			}

			bool operator >(const range &rhs) const{
				return !(*this < rhs || *this == rhs);
			}

			bool contains(const range &rhs) const{
				return (contains(rhs.left_) && contains(rhs.right_));
			}

			bool contains(value_type value) const{
				return (value >= left_ && value <= right_);
			}

			bool intercepts(const range &rhs) const{
				return ((left_ <= rhs.left_ && rhs.left_ <= right_) ||
					(left_ <= rhs.right_ && rhs.right_ <= right_));
			}

			bool is_outside(const range &rhs) const{
				return (rhs.right_ < left_ || right_ < rhs.left_);
			}

			bool is_outside_left(const range &rhs) const{
				return (rhs.right_ < left_);
			}

			bool is_outside_right(const range &rhs) const{
				return (right_ < rhs.left_);
			}

			value_type left() const{
				return left_;
			}

			value_type right() const{
				return right_;
			}

		private:
			value_type left_;
			value_type right_;
		};
	}
}

#endif /* !OOSL_RANGE_H */
