/*
Copyright (C) 2009 Frédéric Zubler, Rodney J. Douglas,
Dennis Göhlsdorf, Toby Weston, Andreas Hauri, Roman Bauer,
Sabina Pfister & Adrian M. Whatley.

This file is part of CX3D.

CX3D is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

CX3D is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with CX3D.  If not, see <http://www.gnu.org/licenses/>.
*/

package ini.cx3d.spatialOrganization;

import ini.cx3d.spatialOrganization.interfaces.Rational;
import ini.cx3d.spatialOrganization.factory.RationalFactory;

import java.math.BigInteger;

/**
 * NOTE: This class has been replaced by a native implementation! It is merely used for debugging purposes!
 *
 * Used to represent numbers as fractions. Each number therefore consists of a numerator and
 * a denominator, both of which are stored as instances of type {@link BigInteger}.
 * 
 * All numbers are stored as canceled fractions. This class provides the functionality to
 * add, subtract, multiply, divide or compare two rational numbers. 
 * 
 * @author Dennis Goehlsdorf
 *
 */
public class RationalJava extends ini.cx3d.swig.simulation.Rational implements Rational {

	/**
	 * The numerator of this rational number.
	 */
	BigInteger numerator;
	
	/**
	 * The denominator of this rational number. 
	 */
	BigInteger denominator;

	/**
	 * Initializes a new rational number from two long integer values.
	 * Both values are transformed internally into numbers of type {@link BigInteger}.
	 * @param numerator The numerator of the new rational.
	 * @param denominator The denominator of the new rational.
	 */
	public RationalJava(long numerator, long denominator) {
		this(BigInteger.valueOf(numerator), BigInteger.valueOf(denominator));
		registerJavaObject(this);
//		detMaxValue(this);
	}

	/**
	 * Initializes a new rational number from two values of type {@link BigInteger}.
	 * @param numerator The numerator of the new rational.
	 * @param denominator The denominator of the new rational.
	 */
	public RationalJava(BigInteger numerator, BigInteger denominator) {
		registerJavaObject(this);
		this.numerator = numerator;
		this.denominator = denominator;
		if (denominator.signum() == -1) {
			this.denominator = this.denominator.negate();
			this.numerator = this.numerator.negate();
		}
	}

	/**
	 * Computes 2^<code>exp</code>.
	 * @param exp The exponent.
	 * @return A <code>BigInteger</code> storing the value of 2^<code>exp</code>.
	 */
	private BigInteger pow2(int exp) {
		BigInteger result = BigInteger.valueOf(1);
		BigInteger temp = BigInteger.valueOf(2);
		while (exp > 0) {
			if ((exp & 1) == 1)
				result = result.multiply(temp);
			temp = temp.multiply(temp);
			exp >>= 1;
		}
		return result;
	}

	/**
	 * Creates a rational number from a <code>double</code> value.
	 * The floating point number is split into mantisse and exponent and these are then
	 * used to calculate numerator and denominator of the rational number.
	 * The procedure used assures that the number is correctly transformed into
	 * the rational number which it represents.
	 * @param value The double value that should be transformed.
	 */
	public RationalJava(double value) {
		long mantisse = Double.doubleToLongBits(value);
		long ex = ((mantisse&0x7ff0000000000000L) >> 52) -1023;
		long sign = mantisse&0x8000000000000000L;

		// lets calculate the mantisse:
		mantisse &=      0x000fffffffffffffL;
		long denom = 0x0010000000000000L;
		// if value is a denormalized value...
		if (ex == -1023) {
			ex++;
			mantisse <<= 1;
		}
		// otherwise, add the denominator to the mantisse:
		else
			mantisse |= denom;

		if (sign != 0)
			mantisse *= -1;
		this.numerator = BigInteger.valueOf(mantisse);
		this.denominator = BigInteger.valueOf(denom);
		if (ex > 0)
			this.numerator = this.numerator.multiply(pow2((int)ex));
		else
			this.denominator = this.denominator.multiply(pow2(-(int)ex));
		cancel();
	}

	/**
	 * Creates a <code>String</code> representation of this rational number.
	 * @return A string in the format "numerator / denominator" if the denominator is different from 1 and a string
	 * in the format "numerator" else.
	 */
	@Override
	public String toString() {
		return String.format("%.5f", doubleValue());
	}

	/**
	 * Divides numerator and denominator of this rational number by their greatest common divisor.
	 */
	protected void cancel() {
		BigInteger gcd = this.numerator.gcd(denominator);
		this.numerator = numerator.divide(gcd);
		this.denominator = denominator.divide(gcd);
	}

	/**
	 * @return <code>true</code>, if this number is equal to 0.
	 */
	@Override
	public boolean isZero() {
		return (numerator.equals(BigInteger.ZERO));
	}

	/**
	 * Negates this rational number. The number itself is changed during this process.
	 * @return A reference to <code><b>this</b></code>.
	 */
	@Override
	public Rational negate() {
		this.numerator = this.numerator.negate();
		return this;
	}

	@Override
	public Rational add(Rational otherValue) {
		if (!(otherValue instanceof RationalJava)) {
			throw new RuntimeException("otherValue must be of type RationalJava");
		}
		return	add((RationalJava) otherValue);
	}

	@Override
	public Rational increaseBy(Rational otherValue) {
		if (!(otherValue instanceof RationalJava)) {
			throw new RuntimeException("otherValue must be of type RationalJava");
		}
		return	increaseBy((RationalJava) otherValue);
	}

	@Override
	public Rational subtract(Rational otherValue) {
		if (!(otherValue instanceof RationalJava)) {
			throw new RuntimeException("otherValue must be of type RationalJava");
		}
		return	subtract((RationalJava) otherValue);
	}

	@Override
	public Rational decreaseBy(Rational otherValue) {
		if (!(otherValue instanceof RationalJava)) {
			throw new RuntimeException("otherValue must be of type RationalJava");
		}
		return	decreaseBy((RationalJava) otherValue);
	}

	@Override
	public Rational multiply(Rational otherValue) {
		if (!(otherValue instanceof RationalJava)) {
			throw new RuntimeException("otherValue must be of type RationalJava");
		}
		return	multiply((RationalJava) otherValue);
	}

	@Override
	public Rational multiplyBy(Rational otherValue) {
		if (!(otherValue instanceof RationalJava)) {
			throw new RuntimeException("otherValue must be of type RationalJava");
		}
		return	multiplyBy((RationalJava) otherValue);
	}

	@Override
	public Rational divide(Rational otherValue) {
		if (!(otherValue instanceof RationalJava)) {
			throw new RuntimeException("otherValue must be of type RationalJava");
		}
		return	divide((RationalJava) otherValue);
	}

	@Override
	public Rational divideBy(Rational otherValue) {
		if (!(otherValue instanceof RationalJava)) {
			throw new RuntimeException("otherValue must be of type RationalJava");
		}
		return	divideBy((RationalJava) otherValue);
	}

	/**
	 * Adds another rational number to this rational and returns a new instance of RationalJava. This number itself
	 * is not modified during this calculation.
	 * @param otherValue The second argument of the addition.
	 * @return A new instance of <code>RationalJava</code> representing the result.
	 */
	private Rational add(RationalJava otherValue) {
		BigInteger gcd = this.denominator.gcd(otherValue.denominator);
		BigInteger otherNonDiv = otherValue.denominator.divide(gcd);
		return new RationalFactory().create(this.numerator.multiply(otherNonDiv).add(
				otherValue.numerator.multiply(this.denominator.divide(gcd))),
				this.denominator.multiply(otherNonDiv));
	}

	/**
	 * Increases this rational by another rational. The number itself is modified during this calculation.
	 * @param otherValue The value by which this rational should be increased.
	 * @return A reference to this object itself.
	 */
	private Rational increaseBy(RationalJava otherValue) {
		BigInteger gcd = this.denominator.gcd(otherValue.denominator);
		BigInteger otherNonDiv = otherValue.denominator.divide(gcd);
		this.numerator = this.numerator.multiply(otherNonDiv).add(
				otherValue.numerator.multiply(this.denominator.divide(gcd)));
		this.denominator = this.denominator.multiply(otherNonDiv);
		return this;
	}

	/**
	 * Subtracts another rational number from this rational and returns a new instance of RationalJava. This number itself
	 * is not modified during this calculation.
	 * @param otherValue The second argument of the subtraction.
	 * @return A new instance of <code>RationalJava</code> representing the result.
	 */
	private Rational subtract(RationalJava otherValue) {
		BigInteger gcd = this.denominator.gcd(otherValue.denominator);
		BigInteger otherNonDiv = otherValue.denominator.divide(gcd);
		return new RationalFactory().create(this.numerator.multiply(otherNonDiv).subtract(
				otherValue.numerator.multiply(this.denominator.divide(gcd))),
				this.denominator.multiply(otherNonDiv));
	}

	/**
	 * Decreases this rational by another rational. The number itself is modified during this calculation.
	 * @param otherValue The value by which this rational should be decreased.
	 * @return A reference to this object itself.
	 */
	private Rational decreaseBy(RationalJava otherValue) {
		BigInteger gcd = this.denominator.gcd(otherValue.denominator);
		BigInteger otherNonDiv = otherValue.denominator.divide(gcd);
		this.numerator = this.numerator.multiply(otherNonDiv).subtract(
				otherValue.numerator.multiply(this.denominator.divide(gcd)));
		this.denominator = this.denominator.multiply(otherNonDiv);
		return this;
	}

	/**
	 * Multiplies another rational number with this rational and returns a new instance of <code>RationalJava</code>. This number itself
	 * is not modified during this calculation.
	 * @param otherValue The second argument of the multiplication.
	 * @return A new instance of <code>RationalJava</code> representing the result.
	 */
	private Rational multiply(RationalJava otherValue) {
		BigInteger myNumOtherDenomGcd = this.numerator.gcd(otherValue.denominator);
		BigInteger otherNumMyDenomGcd = otherValue.numerator.gcd(this.denominator);
		return new RationalFactory().create(
				this.numerator.divide(myNumOtherDenomGcd).multiply(
						otherValue.numerator.divide(otherNumMyDenomGcd)),
				this.denominator.divide(otherNumMyDenomGcd).multiply(
						otherValue.denominator.divide(myNumOtherDenomGcd)));
	}

	/**
	 * Multiplies this rational by another rational number. The number itself is modified during this calculation.
	 * @param otherValue The value by which this rational should be multiplied.
	 * @return A reference to this object itself.
	 */
	private RationalJava multiplyBy(RationalJava otherValue) {
		BigInteger myNumOtherDenomGcd = this.numerator.gcd(otherValue.denominator);
		BigInteger otherNumMyDenomGcd = otherValue.numerator.gcd(this.denominator);
		this.numerator = this.numerator.divide(myNumOtherDenomGcd).multiply(
				otherValue.numerator.divide(otherNumMyDenomGcd));
		this.denominator = this.denominator.divide(otherNumMyDenomGcd).multiply(
				otherValue.denominator.divide(myNumOtherDenomGcd));
		return this;
	}

	/**
	 * Divides this rational number by another rational number and returns a new instance of <code>RationalJava</code>. This number itself
	 * is not modified during this calculation.
	 * @param otherValue The second argument of the division.
	 * @return A new instance of <code>RationalJava</code> representing the result.
	 */
	private Rational divide(RationalJava otherValue) {
		if (otherValue.numerator.equals(BigInteger.ZERO))
			throw new IllegalArgumentException("Attempt to divide by a RationalJava that is 0!");
//		BigInteger myNumOtherDenomGcd = this.numerator.gcd(otherValue.numerator);
//		BigInteger otherNumMyDenomGcd = otherValue.denominator.gcd(this.denominator);
		BigInteger myNumOtherDenomGcd = this.numerator.gcd(otherValue.denominator);
		BigInteger otherNumMyDenomGcd = otherValue.numerator.gcd(this.denominator);
		return new RationalFactory().create(
				this.numerator.divide(myNumOtherDenomGcd).multiply(
						otherValue.denominator.divide(otherNumMyDenomGcd)),
				this.denominator.divide(otherNumMyDenomGcd).multiply(
						otherValue.numerator.divide(myNumOtherDenomGcd)));
	}

	/**
	 * Divides this rational by another rational number. The number itself is modified during this calculation.
	 * @param otherValue The value by which this rational should be divided.
	 * @return A reference to this object itself.
	 */
	private RationalJava divideBy(RationalJava otherValue) {
//		BigInteger myNumOtherDenomGcd = this.numerator.gcd(otherValue.numerator);
//		BigInteger otherNumMyDenomGcd = otherValue.denominator.gcd(this.denominator);
		BigInteger myNumOtherDenomGcd = this.numerator.gcd(otherValue.denominator);
		BigInteger otherNumMyDenomGcd = otherValue.numerator.gcd(this.denominator);
		this.numerator = this.numerator.divide(myNumOtherDenomGcd).multiply(
				otherValue.denominator.divide(otherNumMyDenomGcd));
		this.denominator = this.denominator.divide(otherNumMyDenomGcd).multiply(
				otherValue.numerator.divide(myNumOtherDenomGcd));
//		detMaxValue(this);
		return this;
	}

	/**
	 * Creates a double approximation of this rational.
	 * @return An approximation of this rational. The function internally transforms the numerator and the denominator
	 * into <code>double</code> values and then returns the division of the first by the second.
	 */
	public double doubleValue() {
		return numerator.doubleValue() / denominator.doubleValue();
	}

	/**
	 * {@inheritDoc}
	 */
	public boolean equals(Object obj) {
		if (! (obj instanceof Rational))
			return false;
		else {
			return this.numerator.equals(((RationalJava)obj).numerator)
				&& this.denominator.equals(((RationalJava)obj).denominator);
		}
	}

	/**
	 * Implementation of Comparable.compareTo(Object).
	 * @param obj A RationalJava to which this value should be compared to.
	 * @return -1 if this RationalJava is smaller, 0 if this RationalJava is equal and +1 if it is bigger than the other RationalJava.
	 */

	@Override
	public int compareTo(Rational obj) {
		if (!(obj instanceof RationalJava))
			throw new RuntimeException("Rationals can only be compared to Rationals!");
		return ((RationalJava) this.subtract((RationalJava) obj)).numerator.signum();
	}
}
